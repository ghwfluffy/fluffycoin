#include <fluffycoin/svc/EventPublisher.h>
#include <fluffycoin/svc/Log.h>

#include <fluffycoin/pb/Json.h>
#include <fluffycoin/pb/AnyMessage.h>

#include <fcpb/comm/Event.pb.h>

using namespace fluffycoin;
using namespace fluffycoin::svc;

EventPublisher::EventPublisher(
    const zmq::Context &zmqContext)
        : zmqContext(zmqContext)
{
}

bool EventPublisher::openPort(
    uint16_t port,
    const BinData &serverKey)
{
    Details details(Log::Event);

    // Bind new socket
    socket = std::make_unique<zmq::Publisher>(zmqContext);
    socket->bind(std::string(), port, serverKey, details);

    if (!details.isOk())
        socket.reset();

    return details.isOk();
}

void EventPublisher::publish(
    const std::string &topic,
    const google::protobuf::Message &event)
{
    if (!socket)
    {
        log::error(Log::Event, "Failed to publish event {}: No publisher socket.", topic);
        return;
    }

    // Put in envelope
    fcpb::comm::Event envelope;
    pb::AnyMessage::toAny(event, *envelope.mutable_body());

    // Convert to JSON
    std::string json;
    Details details(Log::Event);
    pb::Json::toJson(envelope, json, details);

    // Publish
    if (details.isOk())
        socket->publish(topic, json, details);
}
