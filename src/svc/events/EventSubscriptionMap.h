#pragma once

#include <fluffycoin/svc/ServiceScene.h>
#include <fluffycoin/svc/Log.h>

#include <fluffycoin/pb/Catalog.h>
#include <fluffycoin/pb/MsgInfo.h>
#include <fluffycoin/pb/AnyMessage.h>

#include <fluffycoin/utils/Details.h>

#include <fluffycoin/log/Log.h>

#include <fcpb/comm/EventEnvelope.pb.h>

#include <map>
#include <string>
#include <memory>
#include <functional>

#include <stdint.h>

namespace fluffycoin::svc
{

/**
 * Holds the map of pub/sub events that the service subscribes to and their handlers
 */
class EventSubscriptionMap
{
    public:
        using EventHandler = std::function<void(
            const ServiceScene &,
            fcpb::comm::EventEnvelope &)>;

        EventSubscriptionMap() = default;
        EventSubscriptionMap(EventSubscriptionMap &&) = default;
        EventSubscriptionMap(const EventSubscriptionMap &) = default;
        EventSubscriptionMap &operator=(EventSubscriptionMap &&) = default;
        EventSubscriptionMap &operator=(const EventSubscriptionMap &) = default;
        ~EventSubscriptionMap() = default;

        const EventHandler *getServiceHandler(
            uint16_t port,
            const std::string &topic) const;

        const EventHandler *getPeerHandler(
            const std::string &topic) const;

        std::list<std::string> getPeerSubscriptions() const;
        std::map<uint16_t, std::list<std::string>> getServiceSubscriptions() const;

        template<typename EventMsg>
        void addServiceSub(
            uint16_t port,
            const std::string &topic,
            std::function<void(const ServiceScene &, EventMsg &)> eventHandler)
        {
            addHandler(
                port,
                topic,
                [port, topic, eventHandler]
                (const ServiceScene &svcScene, fcpb::comm::EventEnvelope &env) -> void
                {
                    // Pull the message data out of the envelope
                    Details details;
                    std::unique_ptr<google::protobuf::Message> msg;
                    pb::AnyMessage::fromAny(env.body(), msg, details);
                    EventMsg *typedMsg = dynamic_cast<EventMsg *>(msg.get());

                    // Failed to deserialize envelope
                    if (!details.isOk())
                    {
                        details.setError(
                            Log::Event,
                            ErrorCode::InvalidMessageFormat,
                            "pubsub",
                            "Failed to deserialize event message type on port {} topic '{}'.",
                            port, topic);
                    }
                    // Deserialized to unexpected type
                    else if (!typedMsg)
                    {
                        details.setError(
                            Log::Event,
                            ErrorCode::InternalError,
                            "pubsub",
                            "Received invalid event message type '{}' on port {} topic '{}'. Expected '{}.",
                            pb::MsgInfo::getType(*msg), port, topic, pb::Catalog::getType<EventMsg>());
                    }
                    // Handle
                    else
                    {
                        eventHandler(svcScene, *typedMsg);
                    }
                });
        }

        template<typename EventMsg>
        void addPeerSub(
            const std::string &topic,
            std::function<void(const ServiceScene &, EventMsg &)> eventHandler)
        {
            addPeerHandler(
                topic,
                [topic, eventHandler]
                (const ServiceScene &svcScene, fcpb::comm::EventEnvelope &env) -> void
                {
                    // Pull the message data out of the envelope
                    Details details;
                    std::unique_ptr<google::protobuf::Message> msg;
                    pb::AnyMessage::fromAny(env.body(), msg, details);
                    EventMsg *typedMsg = dynamic_cast<EventMsg *>(msg.get());

                    // Failed to deserialize envelope
                    if (!details.isOk())
                    {
                        details.setError(
                            Log::Event,
                            ErrorCode::InvalidMessageFormat,
                            "pubsub",
                            "Failed to deserialize peer event message for topic '{}'.",
                            topic);
                    }
                    // Deserialized to unexpected type
                    else if (!typedMsg)
                    {
                        details.setError(
                            Log::Event,
                            ErrorCode::InternalError,
                            "pubsub",
                            "Received invalid peer event message type '{}' for topic '{}'. Expected '{}.",
                            pb::MsgInfo::getType(*msg), topic, pb::Catalog::getType<EventMsg>());
                    }
                    // Handle
                    else
                    {
                        eventHandler(svcScene, *typedMsg);
                    }
                });
        }

    private:
        void addHandler(
            uint16_t port,
            const std::string &topic,
            EventHandler handler);

        void addPeerHandler(
            const std::string &topic,
            EventHandler handler);

        std::map<std::string, EventHandler> mapPeerHandlers;
        std::map<uint16_t, std::map<std::string, EventHandler>> mapServiceHandlers;
};

}
