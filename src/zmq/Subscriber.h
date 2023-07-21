#pragma once

#include <fluffycoin/zmq/Context.h>

#include <fluffycoin/utils/BinData.h>

#include <string>

namespace fluffycoin::zmq
{

/**
 * ZeroMQ TCP subscriber (client) that can connect to a publisher
 * and be notified when events occur.
 */
class Subscriber
{
    public:
        Subscriber(
            const Context &ctx);
        Subscriber(Subscriber &&);
        Subscriber(const Subscriber &) = delete;
        Subscriber &operator=(Subscriber &&);
        Subscriber &operator=(const Subscriber &) = delete;
        ~Subscriber();

        void connect(
            const BinData &serverKey,
            const std::string &host,
            uint16_t port,
            Details &details);

        void connect(
            const std::string &host,
            uint16_t port,
            Details &details);

        void subscribe(
            const std::string &topic,
            Details &details);

        bool recv(
            std::string &topic,
            BinData &data,
            Details &details);

        int getFd() const;

        operator bool() const;

    private:
        const Context *ctx;
        void *socket;
};

}
