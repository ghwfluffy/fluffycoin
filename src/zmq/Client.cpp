#include <fluffycoin/zmq/Client.h>
#include <fluffycoin/zmq/Utils.h>

#include <fluffycoin/utils/Errno.h>

#include <fluffycoin/log/Log.h>

#include <zmq.h>
#include <string.h>

using namespace fluffycoin;
using namespace fluffycoin::zmq;

Client::Client(
    const Context &ctx)
        : ctx(&ctx)
{
    socket = nullptr;
}

Client::Client(Client &&rhs)
{
    ctx = nullptr;
    socket = nullptr;
    operator=(std::move(rhs));
}

Client &Client::operator=(Client &&rhs)
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

Client::~Client()
{
    close();
}

void Client::close()
{
    if (!socket)
        return;

    int ret = zmq_close(socket);
    if (ret != 0)
        log::error(log::Comm, "Failed to close client socket: {}.", Errno::error());
    socket = nullptr;
}

void Client::connect(
    const std::string &host,
    uint16_t port,
    Details &details)
{
    connect(host, port, BinData(), details);
}

void Client::connect(
    const std::string &host,
    uint16_t port,
    const BinData &serverKey,
    Details &details)
{
    // Cleanup any previous socket
    close();

    // Connect socket
    socket = Utils::connectSocket(ctx, ZMQ_REQ, host, port, serverKey, details);
    if (details.isOk())
        details.log().info(log::Comm, "Opened API client connection.");

    // Cleanup on error
    if (!details.isOk())
        close();
}

void Client::send(
    const BinData &data,
    Details &details)
{
    if (!socket)
        details.setError(log::Comm, ErrorCode::NotConnected, "zmq_send", "Not connected to server.");

    if (details.isOk())
    {
        int ret = zmq_send(socket, data.data(), data.length(), 0);
        if (ret == -1)
        {
            details.setError(log::Comm, ErrorCode::WriteError, "zmq_send",
                "Failed to request to server: {}.", Errno::error());
        }
        else if (static_cast<size_t>(ret) != data.length())
        {
            details.setError(log::Comm, ErrorCode::WriteError, "zmq_send",
                "Failed to send full request ({}/{} bytes sent).", ret, data.length());
        }
    }

    // Close socket on error
    if (!details.isOk())
        close();
}

bool Client::recv(
    BinData &data,
    Details &details)
{
    if (!socket)
        details.setError(log::Comm, ErrorCode::NotConnected, "zmq_recv", "Not connected to server.");

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
            details.setError(log::Comm, ErrorCode::ReadError, "zmq_recv",
                "Error while reading response from server: {}.", Errno::error());
        }
        // Received message
        else
        {
            bHasMessage = true;
            data.resize(zmq_msg_size(&msg));
            memcpy(data.data(), zmq_msg_data(&msg), zmq_msg_size(&msg));
        }
    }

    // Close socket on error
    if (!details.isOk())
        close();

    return bHasMessage && details.isOk();
}

int Client::getFd() const
{
    return Utils::getFd(socket);
}

Client::operator bool() const
{
    // There's no real way to know if ZMQ is actively connected
    return socket != nullptr;
}
