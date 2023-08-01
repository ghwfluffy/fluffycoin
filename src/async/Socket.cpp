#include <fluffycoin/async/Socket.h>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>

#include <list>

using namespace fluffycoin;
using namespace fluffycoin::async;

namespace
{

/**
 * Can only tell boost to write one thing at a time.
 * So this tracks concurrent writes to be performed.
 */
struct PendingWrite
{
    Details details;
    BinData data;
    unsigned int timeout = 0;
    std::function<void(Details &)> callback;
};

}

namespace fluffycoin::async
{

// Private implementation
struct SocketImpl
{
    boost::asio::io_context *pasio = nullptr;
    std::mutex mtx;
    std::unique_ptr<boost::asio::posix::stream_descriptor> fd;
    bool bClose = false;
    boost::asio::deadline_timer timer;

    std::list<PendingWrite> writes;

    SocketImpl() = default;
    SocketImpl(
        boost::asio::io_context &asio,
        int iFD,
        bool bClose)
            : pasio(&asio)
            , fd(std::make_unique<boost::asio::posix::stream_descriptor>(asio, iFD))
            , bClose(bClose)
            , timer(asio)
    {}

    ~SocketImpl()
    {
        if (fd && !bClose)
            fd->release();
    }
};

}

namespace
{

void setTimeout(
    SocketImpl &impl,
    unsigned int timeout,
    const std::lock_guard<std::mutex> &lock)
{
    (void)lock;

    if (timeout > 0)
    {
        impl.timer.cancel();
        impl.timer.expires_from_now(boost::posix_time::milliseconds(timeout));
        impl.timer.async_wait(
            [&impl](const boost::system::error_code &error) -> void
            {
                if (!impl.fd || error)
                    return;
                std::lock_guard<std::mutex> lock(impl.mtx);
                impl.fd->cancel();
            });
    }
}

void queueWrite(
    SocketImpl &impl,
    const std::lock_guard<std::mutex> &lock)
{
    // Shouldn't happen
    if (impl.writes.empty())
        return;

    PendingWrite &pending = impl.writes.front();

    // Set timeout
    unsigned int timeout = pending.timeout;
    setTimeout(impl, timeout, lock);

    size_t dataSize = pending.data.length();

    // Write
    boost::asio::async_write(
        *impl.fd,
        boost::asio::buffer(pending.data.data(), dataSize),
        [&impl, timeout, dataSize](const boost::system::error_code &error, size_t bytesWritten) -> void
        {
            // Cleanup impl mem
            PendingWrite pending;
            {
                std::lock_guard<std::mutex> lock(impl.mtx);
                if (timeout > 0)
                    impl.timer.cancel();
                if (!impl.writes.empty())
                {
                    pending = std::move(impl.writes.front());
                    impl.writes.pop_front();
                }
            }

            if (error == boost::asio::error::operation_aborted)
            {
                pending.details.setError(log::Comm, ErrorCode::SocketTimeout,
                    "socket_write", "Timeout trying to write.");
                log::debug("Wrote {}/{} bytes.", bytesWritten, dataSize);
            }
            else if (error)
            {
                pending.details.setError(log::Comm, ErrorCode::SocketError,
                    "socket_write", "Error writing to socket: {}", error.message());
                log::debug("Wrote {}/{} bytes.", bytesWritten, dataSize);
            }
            else if (!pending.callback)
            {
                pending.details.setError(log::Comm, ErrorCode::InternalError,
                    "write_callback", "No write callback to trigger.");
            }
            else
            {
                pending.callback(pending.details);
            }
        });
}

}

Socket::Socket()
{
}

Socket::Socket(
    boost::asio::io_context &asio,
    int iFD,
    bool bClose)
        : impl(std::make_unique<SocketImpl>(asio, iFD, bClose))
{
}

Socket::Socket(Socket &&rhs)
{
    operator=(std::move(rhs));
}

Socket &Socket::operator=(Socket &&rhs)
{
    if (this != &rhs)
        impl = std::move(rhs.impl);
    return (*this);
}

Socket::~Socket()
{
}

Socket::operator bool() const
{
    return impl && impl->fd && impl->fd->is_open();
}

void Socket::read(
    Details &details,
    std::function<void(Details &)> callback,
    unsigned int timeout)
{
    if (!(*this))
    {
        details.setError(log::Comm, ErrorCode::NotConnected, "socket_read", "Not connected.");
        return;
    }

    std::lock_guard<std::mutex> lock(impl->mtx);
    setTimeout(*impl, timeout, lock);

    SocketImpl &implMem = *impl;

    impl->fd->async_wait(
        boost::asio::posix::stream_descriptor::wait_read,
        [&implMem, timeout, details, callback](const boost::system::error_code &error) mutable -> void
        {
            // Stop tracking timeout
            if (timeout > 0)
                implMem.timer.cancel();

            if (error == boost::asio::error::operation_aborted)
            {
                details.setError(log::Level::Info, log::Comm, ErrorCode::SocketTimeout,
                    "socket_read", "Timeout waiting to read.");
            }
            else if (error)
            {
                details.setError(log::Comm, ErrorCode::SocketError,
                    "socket_read", "Error reading from socket: {}", error.message());
            }
            else
            {
                callback(details);
            }
        });
}

void Socket::write(
    Details &details,
    BinData data,
    std::function<void(Details &)> callback,
    unsigned int timeout)
{
    if (!(*this))
    {
        details.setError(log::Comm, ErrorCode::NotConnected, "socket_write", "Not connected.");
        return;
    }

    std::lock_guard<std::mutex> lock(impl->mtx);
    // Add write to queue
    impl->writes.emplace_back();
    PendingWrite &pending = impl->writes.back();
    pending.timeout = timeout;
    pending.details = std::move(details);
    pending.data = std::move(data);
    pending.callback = std::move(callback);

    // Something already writing
    if (impl->writes.size() > 1)
        return;

    queueWrite(*impl, lock);
}

void Socket::wait(
    Details &details,
    std::function<void(Details &)> callback,
    unsigned int timeout)
{
    if (!(*this))
    {
        details.setError(log::Comm, ErrorCode::NotConnected, "socket_connect", "Not connected.");
        return;
    }

    std::lock_guard<std::mutex> lock(impl->mtx);
    setTimeout(*impl, timeout, lock);
    SocketImpl &implMem = *impl;

    impl->fd->async_wait(
        boost::asio::posix::stream_descriptor::wait_write,
        [details, callback, timeout, &implMem](const boost::system::error_code &error) mutable -> void
        {
            // Stop tracking timeout
            if (timeout > 0)
                implMem.timer.cancel();

            if (error == boost::asio::error::operation_aborted)
            {
                details.setError(log::Comm, ErrorCode::SocketTimeout,
                    "socket_connect", "Timeout waiting to connect.");
            }
            else if (error)
            {
                details.setError(log::Comm, ErrorCode::SocketError,
                    "socket_connect", "Error connecting: {}", error.message());
            }
            else
            {
                callback(details);
            }
        });
}
