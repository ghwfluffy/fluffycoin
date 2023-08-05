#include <fluffycoin/zmq/Utils.h>

#include <fluffycoin/ossl/Curve25519.h>
#include <fluffycoin/ossl/Error.h>

#include <fluffycoin/utils/Errno.h>

#include <fluffycoin/log/Log.h>

#include <zmq.h>
#include <sodium.h>
#include <string.h>

using namespace fluffycoin;
using namespace fluffycoin::zmq;

int Utils::getFd(void *socket)
{
    int fd = -1;
    if (socket)
    {
        size_t size = sizeof(fd);
        int ret = zmq_getsockopt(socket, ZMQ_FD, &fd, &size);
        if (ret != 0)
        {
            fd = -1;
            log::error(log::Comm, "Failed to retrieve ZMQ socket: {}.",
                Errno::error());
        }
    }

    return fd;
}

namespace
{

void setupServerCurve(
    void *socket,
    const BinData &serverKey,
    Details &details)
{
    // Sane validate input
    if (serverKey.length() != 32)
    {
        details.setError(log::Comm, ErrorCode::InternalError, "zmq_curve",
            "Invalid server key length {}.", serverKey.length());
    }

    // libSodium wants to have both the private and public components of the ED25519 key
    SafeData extendedKey;
    if (details.isOk())
    {
        extendedKey.resize(64);
        memcpy(extendedKey.data(), serverKey.data(), serverKey.length());
        BinData pubKey = ossl::Curve25519::privateToPublic(serverKey);
        if (pubKey.length() != 32)
        {
            details.setError(log::Comm, ErrorCode::InternalError, "zmq_curve",
                "Failed to convert ED25519 private key to public key: {}.", ossl::Error::pop());
        }
        // Append to private key to get extended key
        else
        {
            memcpy(extendedKey.data() + serverKey.length(), pubKey.data(), pubKey.length());
        }
    }

    // Convert ED25519 key to X25519
    SafeData curveKey;
    if (details.isOk())
    {
        curveKey.resize(32);
        int i = crypto_sign_ed25519_sk_to_curve25519(curveKey.data(), extendedKey.data());
        if (i != 0)
        {
            details.setError(log::Comm, ErrorCode::InternalError, "zmq_curve",
                "Failed to convert ED25519 to X25519 with {}.", i);
        }
    }

    // Get public component from private component
    BinData curvePub;
    if (details.isOk())
    {
        curvePub.resize(32);
        int i = crypto_scalarmult_base(curvePub.data(), curveKey.data());
        if (i != 0)
        {
            details.setError(log::Comm, ErrorCode::InternalError, "zmq_curve",
                "Failed to convert X25519 private key to public key with {}.", i);
        }
    }

    // Setup socket
    if (details.isOk())
    {
        bool bOk = zmq_setsockopt(socket, ZMQ_CURVE_PUBLICKEY, curvePub.data(), curvePub.length()) == 0;
        bOk &= zmq_setsockopt(socket, ZMQ_CURVE_SECRETKEY, curveKey.data(), curveKey.length()) == 0;
        constexpr const int server = static_cast<int>(true);
        bOk &= zmq_setsockopt(socket, ZMQ_CURVE_SERVER, &server, sizeof(server)) == 0;
        if (!bOk)
        {
            details.setError(log::Comm, ErrorCode::InternalError, "zmq_curve",
                "Failed to set CurveMQ parameters on server socket.");
        }
    }
}

}

void *Utils::bindSocket(
    const Context *ctx,
    int socketType,
    const std::string &host,
    uint16_t port,
    const BinData &serverKey,
    Details &details)
{
    // Check for dependency
    if (!ctx)
        details.setError(log::Comm, ErrorCode::InternalError, "zmq_ctx", "No server comm context.");

    // Create new socket
    void *socket = nullptr;
    if (details.isOk())
    {
        socket = zmq_socket(*ctx, socketType);
        if (!socket)
            details.setError(log::Comm, ErrorCode::SocketError, "zmq_socket", "Failed to create server socket.");
    }

    // Setup authentication/encryption
    if (details.isOk() && serverKey.empty())
    {
        log::info(log::Comm, "Binding unprotected socket.");
    }
    else if (details.isOk())
    {
        setupServerCurve(socket, serverKey, details);
    }

    // Bind to requested host/port
    if (details.isOk())
    {
        const char *pszHost = host.empty() ? "*" : host.c_str();
        std::string address = fmt::format("tcp://{}:{}", pszHost, port);
        int ret = zmq_bind(socket, address.c_str());
        if (ret != 0)
        {
            details.setError(log::Comm, ErrorCode::ConnectError, "zmq_bind",
                "Failed to bind to '{}': {}.", address, Errno::error());
        }
        else
        {
            log::info(log::Comm, "Bound server socket to '{}'.", address);
        }
    }

    // Cleanup on error
    if (!details.isOk() && socket)
    {
        zmq_close(socket);
        socket = nullptr;
    }

    return socket;
}


namespace
{

void setupClientCurve(
    void *socket,
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
                "Failed to generate ephemeral X25519 keypair: {}.", Errno::error());
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

}

void *Utils::connectSocket(
    const Context *ctx,
    int socketType,
    const std::string &host,
    uint16_t port,
    const BinData &serverKey,
    Details &details)
{
    // Check for dependency
    if (!ctx)
        details.setError(log::Comm, ErrorCode::InternalError, "zmq_ctx", "No client comm context.");

    // Create new socket
    void *socket = nullptr;
    if (details.isOk())
    {
        socket = zmq_socket(*ctx, socketType);
        if (!socket)
            details.setError(log::Comm, ErrorCode::SocketError, "zmq_socket", "Failed to create new client socket.");
    }

    // Setup authentication/encryption
    if (details.isOk() && serverKey.empty())
    {
        log::info(log::Comm, "Connecting unprotected socket.");
    }
    else if (details.isOk())
    {
        setupClientCurve(socket, serverKey, details);
    }

    // Connect to requested host/port
    if (details.isOk())
    {
        std::string address = fmt::format("tcp://{}:{}", host, port);
        int ret = zmq_connect(socket, address.c_str());
        if (ret != 0)
        {
            details.setError(log::Comm, ErrorCode::ConnectError, "zmq_socket",
                "Failed to connect to '{}': {}.", address, Errno::error());
        }
        else
        {
            // We don't actually know if the connection succeeds
            // The background ZMQ thread performs the actual connection
            log::info(log::Comm, "Initialized connection to '{}'.", address);
        }
    }

    // Cleanup on error
    if (!details.isOk() && socket)
    {
        zmq_close(socket);
        socket = nullptr;
    }

    return socket;
}
