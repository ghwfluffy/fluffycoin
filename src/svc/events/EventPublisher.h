#pragma once

#include <fluffycoin/zmq/Context.h>
#include <fluffycoin/zmq/Publisher.h>

#include <fluffycoin/utils/BinData.h>

#include <google/protobuf/message.h>

#include <stdint.h>

namespace fluffycoin::svc
{

/**
 * Publish events to pub/sub subscribers
 */
class EventPublisher
{
    public:
        EventPublisher(
            const zmq::Context &zmqContext);
        EventPublisher(EventPublisher &&) = default;
        EventPublisher(const EventPublisher &) = default;
        EventPublisher &operator=(EventPublisher &&) = delete;
        EventPublisher &operator=(const EventPublisher &) = delete;
        ~EventPublisher() = default;

        bool openPort(
            uint16_t port,
            const BinData &serverKey = BinData());

        void publish(
            const std::string &topic,
            const google::protobuf::Message &event);

    private:
        const zmq::Context &zmqContext;

        std::unique_ptr<zmq::Publisher> socket;
};

}
