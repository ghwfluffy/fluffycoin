#include <fluffycoin/async/UncontrolledSocket.h>

using namespace fluffycoin;
using namespace fluffycoin::async;

namespace fluffycoin::async
{

struct UncontrolledSocketImpl
{
    Socket socket;
    std::recursive_mutex mtx;
    bool wantRead = false;
    bool queued = false;

    std::function<void(Details &)> callback;
    Details pendingDetails;

    UncontrolledSocketImpl() = default;
    UncontrolledSocketImpl(
        boost::asio::io_context &asio,
        int iFD)
            : socket(asio, iFD)
    {
        wantRead = true;
    }

    UncontrolledSocketImpl &operator=(UncontrolledSocketImpl &&rhs)
    {
        if (this != &rhs)
        {
            queued = rhs.queued;
            wantRead = rhs.wantRead;
            socket = std::move(rhs.socket);
            callback = std::move(rhs.callback);
        }

        return (*this);
    }
};

}

namespace
{

void queueRead(
    const std::unique_lock<std::recursive_mutex> &lock,
    UncontrolledSocketImpl &implMem)
{
    (void)lock;

    implMem.queued = true;
    implMem.socket.read(
        implMem.pendingDetails,
        [&implMem](Details &details) mutable -> void
        {
            // Recursive mutex incase this callback gets triggered
            // immediatley instead of being posted
            std::unique_lock<std::recursive_mutex> lock(implMem.mtx);
            implMem.queued = false;

            // Immediately queue someone else so we don't miss anything
            if (details.isOk() && implMem.socket)
                queueRead(lock, implMem);

            // Is there someone waiting already?
            std::function<void(Details &)> localCB = std::move(implMem.callback);
            if (localCB)
            {
                // There is, let them handle this read
                implMem.wantRead = false;
                lock.unlock();
                localCB(details);
            }
            // There is not, we will let them know when they come back there is something waiting
            else
            {
                implMem.wantRead = true;
                implMem.pendingDetails = std::move(details);
            }
        });
}

}

UncontrolledSocket::UncontrolledSocket()
        : impl(std::make_unique<UncontrolledSocketImpl>())
{
}

UncontrolledSocket::UncontrolledSocket(
    boost::asio::io_context &asio,
    int iFD)
        : impl(std::make_unique<UncontrolledSocketImpl>(asio, iFD))
{
}

UncontrolledSocket::UncontrolledSocket(UncontrolledSocket &&rhs)
{
    operator=(std::move(rhs));
}

UncontrolledSocket::~UncontrolledSocket()
{
    if (impl)
    {
        // Cancel socket async read before cleaning up memory
        impl->socket = Socket();
        impl.reset();
    }
}

UncontrolledSocket &UncontrolledSocket::operator=(UncontrolledSocket &&rhs)
{
    if (this != &rhs)
        impl = std::move(rhs.impl);

    return (*this);
}

UncontrolledSocket::operator bool() const
{
    return impl && impl->socket;
}

void UncontrolledSocket::read(
    Details &details,
    std::function<void(Details &)> callback)
{
    if (!impl)
    {
        details.setError(log::Comm, ErrorCode::ConnectionError, "read", "Null socket.");
        return;
    }

    std::unique_lock<std::recursive_mutex> lock(impl->mtx);

    // Already want the reader callback to trigger
    // Already queued, no need to worry about missing anything
    if (impl->queued && impl->wantRead)
    {
        impl->wantRead = false;
        lock.unlock();
        details += std::move(impl->pendingDetails);
        callback(details);
        return;
    }

    // Already want the reader callback to trigger,
    // but we also need to queue a new callback so we
    // don't miss out on anything
    if (!impl->queued && impl->wantRead && impl->socket)
    {
        impl->wantRead = false;
        queueRead(lock, *impl);
        lock.unlock();
        details += std::move(impl->pendingDetails);
        callback(details);
        return;
    }

    // Socket is not good, just trigger already with an error
    if (!impl->socket)
    {
        details.setError(log::Comm, ErrorCode::ConnectionError, "read", "Connection is not open.");
        callback(details);
        return;
    }

    // Already queued a callback, it will cascade to this one
    impl->pendingDetails = std::move(details);
    impl->callback = std::move(callback);
}
