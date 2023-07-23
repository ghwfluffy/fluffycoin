#pragma once

#include <fluffycoin/svc/EventSubscriptionMap.h>

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
            const ServiceScene &svcScene,
            const zmq::Context &zmqCtx,
            boost::asio::io_context &asio,
            const EventSubscriptionMap &handlers);
        EventProcessor(EventProcessor &&) = default;
        EventProcessor(const EventProcessor &) = default;
        EventProcessor &operator=(EventProcessor &&) = delete;
        EventProcessor &operator=(const EventProcessor &) = delete;
        ~EventProcessor() = default;

        void addPeer(
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
            async::UncontrolledSocket socket;
            zmq::Subscriber subscriber;

            SubSocket() = default;
            SubSocket(
                async::UncontrolledSocket socket,
                zmq::Subscriber subscriber);
        };

        SubSocket startListening(
            zmq::Subscriber &subscriber,
            std::function<const EventSubscriptionMap::EventHandler *(const std::string &)> getHandler,
            Details &details);

        const ServiceScene &svcScene;
        const zmq::Context &zmqCtx;
        boost::asio::io_context &asio;
        const EventSubscriptionMap &handlers;

        std::list<std::string> peerTopics;

        std::mutex mtx;
        std::list<SubSocket> localSockets;
        std::map<std::string, SubSocket> peerSockets;
};

}