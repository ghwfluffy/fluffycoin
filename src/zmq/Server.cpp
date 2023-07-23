#include <fluffycoin/zmq/Server.h>
#include <fluffycoin/zmq/Utils.h>

#include <fluffycoin/log/Log.h>

#include <zmq.h>
#include <string.h>

using namespace fluffycoin;
using namespace fluffycoin::zmq;

Server::Server(
    const Context &ctx)
        : ctx(&ctx)
{
    socket = nullptr;
}

Server::Server(Server &&rhs)
{
    ctx = nullptr;
    socket = nullptr;
    operator=(std::move(rhs));
}

Server &Server::operator=(Server &&rhs)
{
    if (this != &rhs)
    {
        close();
        ctx = rhs.ctx;
        socket = rhs.socket;
        rhs.socket = nullptr;
    }

    return (*this);
}

Server::~Server()
{
    close();
}

void Server::close()
{
    std::lock_guard<std::mutex> lock(mtx);
    close(lock);
}

void Server::close(
    const std::lock_guard<std::mutex> &lock)
{
    (void)lock;

    if (!socket)
        return;

    int ret = zmq_close(socket);
    if (ret != 0)
        log::error(log::Comm, "Failed to close server socket ({}): {}.", errno, strerror(errno));
    socket = nullptr;
}

void Server::bind(
    const std::string &host,
    uint16_t port,
    Details &details)
{
    bind(host, port, SafeData(), details);
}

void Server::bind(
    const std::string &host,
    uint16_t port,
    const BinData &serverKey, // ED25519 private key
    Details &details)
{
    std::lock_guard<std::mutex> lock(mtx);

    // Cleanup any previous socket
    close(lock);

    // Bind socket
    socket = Utils::bindSocket(ctx, ZMQ_ROUTER, host, port, serverKey, details);
    if (details.isOk())
        details.log().info(log::Comm, "Opened API port.");

    // Cleanup on error
    if (!details.isOk())
        close(lock);
}

bool Server::recv(
    ClientId &client,
    BinData &data,
    Details &details)
{
    std::lock_guard<std::mutex> lock(mtx);

    if (!socket)
        details.setError(log::Comm, ErrorCode::NotConnected, "zmq_server", "No server socket.");

    // Read first part (should be client ID)
    zmq_msg_t msg = {};
    bool bHasMessage = false;
    if (details.isOk())
    {
        zmq_msg_init(&msg);
        int ret = zmq_recvmsg(socket, &msg, ZMQ_DONTWAIT);
        // Full message not ready
        if (ret == -1 && errno == EAGAIN)
        { }
        // Error
        else if (ret == -1)
        {
            details.setError(log::Comm, ErrorCode::SocketError, "zmq_server", "Failed to read client ID: {}.", strerror(errno));
        }
        // First message is client ID
        else
        {
            bHasMessage = true;
            client.resize(zmq_msg_size(&msg));
            memcpy(client.data(), zmq_msg_data(&msg), zmq_msg_size(&msg));
        }
    }

    // Read second part (should be zeros)
    if (details.isOk() && bHasMessage)
    {
        int ret = zmq_recvmsg(socket, &msg, ZMQ_DONTWAIT);
        // Error
        if (ret == -1)
        {
            details.setError(log::Comm, ErrorCode::SocketError, "zmq_server", "Failed to read delimiter frame: {}.", strerror(errno));
        }
        else if (ret != 0)
        {
            details.setError(log::Comm, ErrorCode::ProtocolError, "zmq_router", "Expected null frame but received {} bytes.", ret);
            // Pop last message
            ret = zmq_recvmsg(socket, &msg, ZMQ_DONTWAIT);
            if (ret >= 0)
                log::debug(log::Comm, "Read {} bytes after invalid zmq second router frame.", ret);
            else
                log::debug(log::Comm, "Received {} ({}) when trying to pop third router frame.", errno, strerror(errno));
        }
    }

    // Read message data
    if (details.isOk() && bHasMessage)
    {
        int ret = zmq_recvmsg(socket, &msg, ZMQ_DONTWAIT);
        // Error
        if (ret == -1)
        {
            details.setError(log::Comm, ErrorCode::SocketError, "zmq_server", "Failed to read data frame: {}.", strerror(errno));
        }
        // Read in message
        else
        {
            data.resize(zmq_msg_size(&msg));
            memcpy(data.data(), zmq_msg_data(&msg), zmq_msg_size(&msg));
        }
    }

    // If we have a message the client ID shouldn't be empty
    if (details.isOk() && bHasMessage && client.empty())
        details.setError(log::Comm, ErrorCode::ProtocolError, "zmq_clientid", "Empty ZMQ client ID.");

    return bHasMessage && details.isOk();
}

void Server::reply(
    const ClientId &client,
    const BinData &data,
    Details &details)
{
    std::lock_guard<std::mutex> lock(mtx);

    // Connected?
    if (!socket)
        details.setError(log::Comm, ErrorCode::NotConnected, "zmq_reply", "No server socket.");

    // Valid input?
    if (details.isOk() && client.empty())
        details.setError(log::Comm, ErrorCode::InternalError, "zmq_reply_clientid", "No client ID provided.");

    // Send client ID first
    unsigned int sent = 0;
    if (details.isOk())
    {
        int ret = zmq_send(socket, client.data(), client.size(), ZMQ_SNDMORE);
        sent += ret >= 0;
        if (ret == -1)
        {
            details.setError(log::Comm, ErrorCode::WriteError, "zmq_reply", "Failed to send reply client ID: {}.", strerror(errno));
        }
        else if (static_cast<size_t>(ret) != client.size())
        {
            details.setError(log::Comm, ErrorCode::WriteError, "zmq_reply",
                "Failed to send full client ID ({}/{} bytes sent).", ret, client.size());
        }
    }

    // Send null frame
    if (details.isOk())
    {
        int ret = zmq_send(socket, nullptr, 0, ZMQ_SNDMORE);
        sent += ret >= 0;
        // Error
        if (ret == -1)
        {
            details.setError(log::Comm, ErrorCode::WriteError, "zmq_reply", "Failed to send delimiter frame: {}.", strerror(errno));
        }
        // That's odd
        else if (ret > 0)
        {
            log::notice(log::Comm, "Sent {} byte null frame.", ret);
        }
    }

    // Send data
    if (details.isOk())
    {
        int ret = zmq_send(socket, data.data(), data.length(), 0);
        sent += ret >= 0;
        if (ret == -1)
        {
            details.setError(log::Comm, ErrorCode::WriteError, "zmq_reply", "Failed to send reply: {}.", strerror(errno));
        }
        else if (static_cast<size_t>(ret) != data.length())
        {
            details.setError(log::Comm, ErrorCode::WriteError, "zmq_reply",
                "Failed to send full reply ({}/{} bytes sent).", ret, data.length());
        }
    }

    // Send missing frames
    while (sent > 0 && sent < 3)
    {
        int ret = zmq_send(socket, nullptr, 0, ++sent != 3 ? ZMQ_SNDMORE : 0);
        if (ret == -1)
            log::error(log::Comm, "Received {} ({}) when sending failed reply frames.", errno, strerror(errno));
        else if (ret != 0)
            log::notice(log::Comm, "Sent {} byte missing null frame.", ret);
    }
}

int Server::getFd() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex &>(mtx));
    return Utils::getFd(socket);
}
