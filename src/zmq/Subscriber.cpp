#include <fluffycoin/zmq/Subscriber.h>
#include <fluffycoin/zmq/Utils.h>

#include <zmq.h>

using namespace fluffycoin;
using namespace fluffycoin::zmq;

Subscriber::Subscriber(
    const Context &ctx)
        : ctx(&ctx)
{
    socket = nullptr;
    subscribed = false;
}

Subscriber::Subscriber(Subscriber &&rhs)
{
    ctx = nullptr;
    socket = nullptr;
    subscribed = false;
    operator=(std::move(rhs));
}

Subscriber &Subscriber::operator=(Subscriber &&rhs)
{
    if (this != &rhs)
    {
        close();
        ctx = rhs.ctx;
        socket = rhs.socket;
        subscribed = rhs.subscribed;
        rhs.socket = nullptr;
    }

    return (*this);
}

Subscriber::~Subscriber()
{
    close();
}

void Subscriber::close()
{
    std::lock_guard<std::mutex> lock(mtx);
    close(lock);
}

void Subscriber::close(
    const std::lock_guard<std::mutex> &lock)
{
    (void)lock;

    if (!socket)
        return;

    int ret = zmq_close(socket);
    if (ret != 0)
        log::error(log::Comm, "Failed to close subscription socket ({}): {}.", errno, strerror(errno));
    socket = nullptr;
    subscribed = false;
}

void Subscriber::connect(
    const std::string &host,
    uint16_t port,
    Details &details)
{
    connect(host, port, BinData(), details);
}

void Subscriber::connect(
    const std::string &host,
    uint16_t port,
    const BinData &serverKey,
    Details &details)
{
    // Cleanup any previous socket
    std::lock_guard<std::mutex> lock(mtx);
    close(lock);

    // Connect socket
    socket = Utils::connectSocket(ctx, ZMQ_SUB, host, port, serverKey, details);
    if (details.isOk())
        details.log().info(log::Comm, "Opened API subscription connection.");

    // Cleanup on error
    if (!details.isOk())
        close(lock);
}

void Subscriber::subscribe(
    const std::string &topic,
    Details &details)
{
    // Cleanup any previous socket
    std::lock_guard<std::mutex> lock(mtx);

    // Check connected
    if (!socket)
        details.setError(log::Comm, ErrorCode::NotConnected, "zmq_subscribe", "Not connected.");

    // Subscribe to topic
    if (details.isOk())
    {
        int ret = zmq_setsockopt(socket, ZMQ_SUBSCRIBE, topic.c_str(), topic.length());
        if (ret != 0)
        {
            details.setError(
                log::Comm,
                ErrorCode::WriteError, "zmq_subscribe",
                "Failed to subscribe to topic '{}': ({}) {}.", topic, errno, strerror(errno));
        }
    }

    // Track subscription state
    if (details.isOk())
        subscribed = true;
}

void Subscriber::subscribeAll(
    Details &details)
{
    subscribe(std::string(), details);
}

bool Subscriber::recv(
    std::string &topic,
    BinData &data,
    Details &details)
{
    // Cleanup any previous socket
    std::lock_guard<std::mutex> lock(mtx);

    // Check connected
    if (!socket)
        details.setError(log::Comm, ErrorCode::NotConnected, "zmq_event", "Not connected.");

    // Prevent programmer error
    if (!subscribed)
        details.setError(log::Comm, ErrorCode::InternalError, "zmq_event", "Not subscribed.");

    // Read topic
    bool bHasMessage = false;
    if (details.isOk())
    {
        zmq_msg_t msg = {};
        zmq_msg_init(&msg);
        int ret = zmq_recvmsg(socket, &msg, ZMQ_DONTWAIT);
        // No message ready yet
        if (ret == -1 && errno == EAGAIN)
        { }
        // Error
        else if (ret == -1)
        {
            details.setError(log::Comm, ErrorCode::ReadError, "zmq_event",
                "Error while reading subscription topic from server: {}.", strerror(errno));
        }
        // Received topic
        else
        {
            bHasMessage = true;
            topic.resize(zmq_msg_size(&msg));
            memcpy(topic.data(), zmq_msg_data(&msg), zmq_msg_size(&msg));
        }
    }

    // Read message
    if (details.isOk() && bHasMessage)
    {
        zmq_msg_t msg = {};
        zmq_msg_init(&msg);
        int ret = zmq_recvmsg(socket, &msg, ZMQ_DONTWAIT);
        // Error
        if (ret == -1)
        {
            details.setError(log::Comm, ErrorCode::ReadError, "zmq_event",
                "Error while reading subscription from server: {}.", strerror(errno));
        }
        // Received topic
        else
        {
            data.resize(zmq_msg_size(&msg));
            memcpy(data.data(), zmq_msg_data(&msg), zmq_msg_size(&msg));
        }
    }

    return details.isOk() && bHasMessage;
}

int Subscriber::getFd() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex &>(mtx));
    return Utils::getFd(socket);
}

Subscriber::operator bool() const
{
    return bool(socket);
}
