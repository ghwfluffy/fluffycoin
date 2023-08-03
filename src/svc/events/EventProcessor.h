#pragma once

#include <fluffycoin/svc/EventSubscriptionMap.h>
#include <fluffycoin/svc/ServiceScene.h>

#include <fluffycoin/zmq/Context.h>
#include <fluffycoin/zmq/Subscriber.h>

#include <fluffycoin/async/boost.h>
#include <fluffycoin/async/UncontrolledSocket.h>

#include <fluffycoin/utils/BinData.h>

#include <mutex>

namespace fluffycoin::svc
{

/**
 * Register pub/sub subscriptions and handle events
 * when they trigger
 */
class EventProcessor
{
    public:
        EventProcessor(
            const ServiceScene &ctx,
            const zmq::Context &zmqCtx,
            const EventSubscriptionMap &handlers);
        EventProcessor(EventProcessor &&) = default;
        EventProcessor(const EventProcessor &) = default;
        EventProcessor &operator=(EventProcessor &&) = delete;
        EventProcessor &operator=(const EventProcessor &) = delete;
        ~EventProcessor() = default;

        bool addPeer(
            const BinData &serverKey,
            const std::string &host,
            uint16_t port);

        void removePeer(
            const std::string &host,
            uint16_t port);

        // Subscribe local service events
        bool subscribeLocal();

    private:
        struct SubSocket
        {
            std::unique_ptr<async::UncontrolledSocket> psocket;
            std::unique_ptr<zmq::Subscriber> psubscriber;

            SubSocket() = default;
            SubSocket(
                std::unique_ptr<async::UncontrolledSocket> psocket,
                std::unique_ptr<zmq::Subscriber> psubscriber);
        };

        SubSocket startListening(
            zmq::Subscriber &subscriber,
            std::function<const EventSubscriptionMap::EventHandler *(const std::string &)> getHandler,
            Details &details);

        const ServiceScene &ctx;
        const zmq::Context &zmqCtx;
        const EventSubscriptionMap &handlers;

        std::list<std::string> peerTopics;

        std::mutex mtx;
        std::list<SubSocket> localSockets;
        std::map<std::string, SubSocket> peerSockets;
};

}
