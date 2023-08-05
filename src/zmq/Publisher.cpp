#include <fluffycoin/zmq/Publisher.h>
#include <fluffycoin/zmq/Utils.h>

#include <fluffycoin/utils/Errno.h>

#include <zmq.h>
#include <string.h>

using namespace fluffycoin;
using namespace fluffycoin::zmq;

Publisher::Publisher(
    const Context &ctx)
        : ctx(&ctx)
{
    socket = nullptr;
}

Publisher::Publisher(Publisher &&rhs)
{
    ctx = nullptr;
    socket = nullptr;
    operator=(std::move(rhs));
}

Publisher &Publisher::operator=(Publisher &&rhs)
{
    if (this != &rhs)
    {
        close();
        socket = rhs.socket;
        ctx = rhs.ctx;
        rhs.socket = nullptr;
    }

    return (*this);
}

Publisher::~Publisher()
{
    close();
}

void Publisher::close()
{
    std::lock_guard<std::mutex> lock(mtx);
    close(lock);
}

void Publisher::close(
    const std::lock_guard<std::mutex> &lock)
{
    (void)lock;

    if (!socket)
        return;

    int ret = zmq_close(socket);
    if (ret != 0)
        log::error(log::Comm, "Failed to close publisher socket: {}.", Errno::error());
    socket = nullptr;
}

void Publisher::bind(
    const std::string &host,
    uint16_t port,
    Details &details)
{
    bind(host, port, BinData(), details);
}

void Publisher::bind(
    const std::string &host,
    uint16_t port,
    const BinData &serverKey,
    Details &details)
{
    std::lock_guard<std::mutex> lock(mtx);

    // Cleanup any previous socket
    close(lock);

    // Bind socket
    socket = Utils::bindSocket(ctx, ZMQ_PUB, host, port, serverKey, details);
    if (details.isOk())
        details.log().info(log::Comm, "Opened event port.");

    // Cleanup on error
    if (!details.isOk())
        close(lock);
}

void Publisher::publish(
    const std::string &topic,
    const BinData &data,
    Details &details)
{
    std::lock_guard<std::mutex> lock(mtx);

    // Connected?
    if (!socket)
        details.setError(log::Comm, ErrorCode::NotConnected, "zmq_publish", "No event socket.");

    // Valid input?
    if (details.isOk() && topic.empty())
        details.setError(log::Comm, ErrorCode::InternalError, "zmq_publish", "Empty topic.");
    if (details.isOk() && data.empty())
        details.setError(log::Comm, ErrorCode::InternalError, "zmq_publish", "No event data.");

    // Send topic
    unsigned int sent = 0;
    if (details.isOk())
    {
        int ret = zmq_send(socket, topic.data(), topic.size(), ZMQ_SNDMORE);
        sent += ret >= 0;
        if (ret == -1)
        {
            details.setError(
                log::Comm,
                ErrorCode::WriteError, "zmq_publish",
                "Failed to publish topic '{}': {}.", topic, Errno::error());
        }
        else if (static_cast<size_t>(ret) != topic.size())
        {
            details.setError(
                log::Comm,
                ErrorCode::WriteError, "zmq_topic",
                "Failed to send full topic '{}' ({}/{} bytes sent).", topic, ret, topic.size());
        }
    }

    // Send event
    if (details.isOk())
    {
        int ret = zmq_send(socket, data.data(), data.length(), 0);
        sent += ret >= 0;
        if (ret == -1)
        {
            details.setError(
                log::Comm,
                ErrorCode::WriteError, "zmq_publish",
                "Failed to send event '{}': {}.", topic, Errno::error());
        }
        else if (static_cast<size_t>(ret) != data.length())
        {
            details.setError(
                log::Comm,
                ErrorCode::WriteError, "zmq_publish",
                "Failed to send full event '{}' ({}/{} bytes sent).", topic, ret, data.length());
        }
    }

    // Send missing frame
    if (sent == 1)
    {
        int ret = zmq_send(socket, nullptr, 0, 0);
        if (ret == -1)
            log::error(log::Comm, "Failed sending missing event frame: {}.", Errno::error());
        else if (ret != 0)
            log::notice(log::Comm, "Sent {} byte missing null frame.", ret);
    }

    // Log
    if (details.isOk())
        log::traffic(log::Comm, "Published event to topic '{}'.", topic);
}

int Publisher::getFd() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex &>(mtx));
    return Utils::getFd(socket);
}
