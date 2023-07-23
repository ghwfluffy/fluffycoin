#pragma once

#include <fluffycoin/zmq/Context.h>

#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/utils/Details.h>

#include <mutex>
#include <string>

namespace fluffycoin::zmq
{

/**
 * ZeroMQ TCP event publisher (server) that clients can subscribe to
 * be notified when events occur.
 */
class Publisher
{
    public:
        Publisher(
            const Context &ctx);
        Publisher(Publisher &&);
        Publisher(const Publisher &) = delete;
        Publisher &operator=(Publisher &&);
        Publisher &operator=(const Publisher &) = delete;
        ~Publisher();

        void bind(
            const std::string &host,
            uint16_t port,
            Details &details);

        // CurveZMQ
        void bind(
            const std::string &host,
            uint16_t port,
            const BinData &serverKey,
            Details &details);

        void publish(
            const std::string &topic,
            const BinData &data,
            Details &details);

        int getFd() const;

    private:
        void close();
        void close(
            const std::lock_guard<std::mutex> &lock);

        const Context *ctx;
        void *socket;
        std::mutex mtx;
};

}
