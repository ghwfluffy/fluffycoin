#include <fluffycoin/svc/EventProcessor.h>
#include <fluffycoin/svc/Log.h>

#include <fluffycoin/pb/Json.h>

#include <boost/asio/post.hpp>
#include <boost/asio/io_context.hpp>

using namespace fluffycoin;
using namespace fluffycoin::svc;

namespace
{

void queueRead(
    async::UncontrolledSocket &socket,
    zmq::Subscriber &subscriber,
    const svc::ServiceScene &ctx,
    std::function<const EventSubscriptionMap::EventHandler *(const std::string &)> getHandler,
    Details &details)
{
    socket.read(
        details,
        [&socket, &subscriber, &ctx, getHandler](Details &details) mutable -> void
        {
            // Socket closing
            if (!details.isOk())
                return;

            // Get data from publisher
            BinData data;
            std::string topic;
            if (details.isOk())
                subscriber.recv(topic, data, details);

            // Sane validate we got something if a message came across
            if (details.isOk() && !topic.empty() && data.empty())
            {
                details.setError(
                    Log::Event,
                    ErrorCode::ReadError, "pubsub",
                    "Received empty data for topic {} subscription.", topic);
            }
            else if (details.isOk() && topic.empty() && !data.empty())
            {
                details.setError(
                    Log::Event,
                    ErrorCode::ReadError,
                    "pubsub",
                    "Received data for null topic subscription.");
            }

            // Deserialize wrapper
            std::unique_ptr<fcpb::comm::Event> envelope;
            if (details.isOk() && !data.empty())
            {
                envelope = std::make_unique<fcpb::comm::Event>();
                pb::Json::fromJson(data.data(), data.length(), *envelope, details);
            }

            // Get handler
            const EventSubscriptionMap::EventHandler *handler = nullptr;
            if (details.isOk() && !data.empty())
            {
                handler = getHandler(topic);
                if (!handler)
                {
                    details.setError(
                        ErrorCode::UnknownCommand,
                        "pubsub",
                        "Failed to find handler for topic {}.", topic);
                }
            }

            // Queue asynchronous handler
            if (details.isOk() && handler)
            {
                boost::asio::post(
                    ctx.asio,
                    [handler, &ctx, envelope = std::move(envelope)]() mutable -> void
                    {
                        (*handler)(ctx, *envelope);
                    });
            }

            // Requeue the socket read
            Details qDetails;
            queueRead(socket, subscriber, ctx, getHandler, qDetails);
        });
}

}

EventProcessor::EventProcessor(
    const ServiceScene &ctx,
    const zmq::Context &zmqCtx,
    const EventSubscriptionMap &handlers)
        : ctx(ctx)
        , zmqCtx(zmqCtx)
        , handlers(handlers)
{
}

void EventProcessor::addPeer(
    const BinData &serverKey,
    const std::string &host,
    uint16_t port)
{
    std::lock_guard<std::mutex> lock(mtx);

    std::string peer = fmt::format("{}:{}", host, port);

    // Are we already subscribed to this peer?
    auto iter = peerSockets.find(peer);
    if (iter != peerSockets.end())
    {
        log::error(Log::Event, "Already subscribed to peer {}.", peer);
        return;
    }

    // Connect to peer
    Details details;
    zmq::Subscriber subscriber(zmqCtx);
    subscriber.connect(host, port, serverKey, details);

    // Subscribe to topics
    for (const std::string &topic : peerTopics)
        subscriber.subscribe(topic, details);

    // Give to asio
    SubSocket sub = startListening(
        subscriber,
        std::bind(&EventSubscriptionMap::getPeerHandler, &handlers, std::placeholders::_1),
        details);

    // Save socket
    if (details.isOk())
        peerSockets.emplace(peer, std::move(sub));

    // Log error
    if (!details.isOk())
        details.setError(Log::Event, ErrorCode::ConnectError, "subscribe", "Failed to subscribe to peer {}.", peer);
}

void EventProcessor::removePeer(
    const std::string &host,
    uint16_t port)
{
    std::string peer = fmt::format("{}:{}", host, port);
    peerSockets.erase(peer);
}

bool EventProcessor::subscribeLocal()
{
    std::map<uint16_t, std::list<std::string>> subs = handlers.getServiceSubscriptions();
    this->peerTopics = handlers.getPeerSubscriptions();

    Details details;
    for (const auto &pair : subs)
    {
        uint16_t port = pair.first;
        const std::list<std::string> &topics = pair.second;

        // Connect to publisher
        zmq::Subscriber subscriber(zmqCtx);
        subscriber.connect("127.0.0.1", port, details);

        // Subscribe to each topic
        for (const std::string &topic : topics)
            subscriber.subscribe(topic, details);

        // Give to asio
        SubSocket sub = startListening(
            subscriber,
            std::bind(&EventSubscriptionMap::getServiceHandler, &handlers, port, std::placeholders::_1),
            details);

        // Any local service fails to setup subscription is fatal
        if (!details.isOk())
            break;

        localSockets.push_back(std::move(sub));
    }

    return details.isOk();
}

EventProcessor::SubSocket EventProcessor::startListening(
    zmq::Subscriber &subscriber,
    std::function<const EventSubscriptionMap::EventHandler *(const std::string &)> getHandler,
    Details &details)
{
    // Hand off to ASIO
    async::UncontrolledSocket socket;
    if (details.isOk())
    {
        socket = async::UncontrolledSocket(ctx.asio, subscriber.getFd());
        queueRead(socket, subscriber, ctx, getHandler, details);
    }

    return SubSocket(std::move(socket), std::move(subscriber));
}
