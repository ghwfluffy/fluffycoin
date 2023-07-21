#pragma once

#include <fluffycoin/zmq/Context.h>

#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/utils/Details.h>

#include <string>

namespace fluffycoin::zmq
{

/**
 * ZeroMQ TCP client that can send messages synchronously to a single server.
 * This class is not reentrant. This class cannot send multiple requests concurrently.
 * This class does not resolve hostnames. They must be resolved before calling connect.
 */
class Client
{
    public:
        Client(
            Context &ctx);
        Client(Client &&);
        Client(const Client &) = delete;
        Client &operator=(Client &&);
        Client &operator=(const Client &) = delete;
        ~Client();

        void connect(
            const std::string &host,
            uint16_t port,
            Details &details);

        // CurveZMQ
        void connect(
            const std::string &host,
            uint16_t port,
            const BinData &serverKey,
            Details &details);

        void send(
            const BinData &data,
            Details &details);

        bool recv(
            BinData &data,
            Details &details);

        int getFd() const;

        operator bool() const;

    private:
        void close();

        void setupCurve(
            const BinData &serverKey,
            Details &details);

        Context *ctx;
        void *socket;
};

}
