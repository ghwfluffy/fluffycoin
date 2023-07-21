#pragma once

#include <fluffycoin/zmq/Context.h>
#include <fluffycoin/zmq/ClientId.h>

#include <fluffycoin/utils/Details.h>
#include <fluffycoin/utils/SafeData.h>

#include <mutex>
#include <string>

namespace fluffycoin::zmq
{

/**
 * ZeroMQ TCP server that can handle requests from multiple clients
 */
class Server
{
    public:
        Server(
            Context &ctx);
        Server(Server &&);
        Server(const Server &) = delete;
        Server &operator=(Server &&);
        Server &operator=(const Server &) = delete;
        ~Server();

        void bind(
            const std::string &host,
            uint16_t port,
            Details &details);

        // CurveZMQ
        void bind(
            const std::string &host,
            uint16_t port,
            const SafeData &serverKey,
            Details &details);

        bool recv(
            ClientId &client,
            BinData &data,
            Details &details);

        void reply(
            const ClientId &client,
            const BinData &data,
            Details &details);

        int getFd() const;

    private:
        void close();
        void close(
            const std::lock_guard<std::mutex> &lock);

        void setupCurve(
            const SafeData &serverKey,
            Details &details);

        Context *ctx;
        void *socket;
        std::mutex mtx;
};

}
