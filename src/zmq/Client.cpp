#include <fluffycoin/zmq/Client.h>
#include <fluffycoin/log/Log.h>

#include <string.h>

#include <zmq.h>
#include <sodium.h>

using namespace fluffycoin;
using namespace fluffycoin::zmq;

Client::Client(
    Context &ctx)
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
        log::error(log::Comm, "Failed to close client socket ({}): {}.", errno, strerror(errno));
    socket = nullptr;
}

void Client::setupCurve(
    const BinData &serverKey,
    Details &details)
{
    // Sane validate input
    if (serverKey.length() != 32)
    {
        details.setError(log::Comm, ErrorCode::InternalError, "zmq_curve",
            "Invalid server key length {}.", serverKey.length());
    }

    // Convert ED25519 key to X25519
    BinData serverCurveKey;
    if (details.isOk())
    {
        serverCurveKey.resize(32);
        int i = crypto_sign_ed25519_pk_to_curve25519(serverCurveKey.data(), serverKey.data());
        if (i != 0)
        {
            details.setError(log::Comm, ErrorCode::InternalError, "zmq_curve",
                "Failed to convert ED25519 to X25519 with {}.", i);
        }
    }

    // Generate ephemeral client keypair
    char szEphemeralPub[42] = {};
    char szEphemeralPriv[42] = {};
    if (details.isOk())
    {
        int i = zmq_curve_keypair(szEphemeralPub, szEphemeralPriv);
        if (i != 0)
        {
            details.setError(log::Comm, ErrorCode::InternalError, "zmq_curve",
                "Failed to generate ephemeral X25519 keypair: {}.", strerror(errno));
        }
    }

    // Setup socket
    if (details.isOk())
    {
        bool bOk = zmq_setsockopt(socket, ZMQ_CURVE_PUBLICKEY, szEphemeralPub, strlen(szEphemeralPub)) == 0;
        bOk &= zmq_setsockopt(socket, ZMQ_CURVE_SECRETKEY, szEphemeralPriv, strlen(szEphemeralPriv)) == 0;
        bOk &= zmq_setsockopt(socket, ZMQ_CURVE_SERVERKEY, serverCurveKey.data(), serverCurveKey.length()) == 0;
        if (!bOk)
        {
            details.setError(log::Comm, ErrorCode::InternalError, "zmq_curve",
                "Failed to set CurveMQ parameters on server socket.");
        }
    }
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
    close();

    // Check for dependency
    if (!ctx)
        details.setError(log::Comm, ErrorCode::InternalError, "zmq_ctx", "No client comm context.");

    // Create new socket
    if (details.isOk())
    {
        socket = zmq_socket(*ctx, ZMQ_REQ);
        if (!socket)
            details.setError(log::Comm, ErrorCode::SocketError, "zmq_socket", "Failed to create new request socket.");
    }

    // Setup authentication/encryption
    if (details.isOk() && serverKey.empty())
    {
        log::info("Connecting unprotected socket.");
    }
    else if (details.isOk())
    {
        setupCurve(serverKey, details);
    }

    // Connect to requested host/port
    if (details.isOk())
    {
        std::string address = fmt::format("tcp://{}:{}", host, port);
        int ret = zmq_connect(socket, address.c_str());
        if (ret != 0)
        {
            details.setError(log::Comm, ErrorCode::ConnectError, "zmq_socket",
                "Failed to connect to '{}': {}.", address, strerror(errno));
        }
        else
        {
            // We don't actually know if the connection succeeds
            // The background ZMQ thread performs the actual connection
            log::info(log::Comm, "Initialized connection to '{}'.", address);
        }
    }

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
                "Failed to request to server: {}.", strerror(errno));
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
                "Error while reading response from server: {}.", strerror(errno));
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
    int fd = -1;
    if (socket)
    {
        size_t size = sizeof(fd);
        int ret = zmq_getsockopt(socket, ZMQ_FD, &fd, &size);
        if (ret != 0)
        {
            fd = -1;
            log::error(log::Comm, "Failed to retrieve ZMQ client socket: ({}) {}.", errno, strerror(errno));
        }
    }

    return fd;
}

Client::operator bool() const
{
    // There's no real way to know if ZMQ is actively connected
    return socket != nullptr;
}
