#pragma once

#include <fluffycoin/zmq/Context.h>

#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/utils/Details.h>

#include <mutex>
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
            const std::string &host,
            uint16_t port,
            Details &details);

        void connect(
            const std::string &host,
            uint16_t port,
            const BinData &serverKey,
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
        void close();
        void close(
            const std::lock_guard<std::mutex> &lock);

        std::mutex mtx;
        const Context *ctx;
        void *socket;
};

}
