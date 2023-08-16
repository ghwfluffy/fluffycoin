#include "fluffytest.h"

#include <fluffycoin/svc/ApiPort.h>
#include <fluffycoin/async/Scheduler.h>

#include <fluffycoin/zmq/Client.h>

#include <fluffycoin/pb/Json.h>
#include <fluffycoin/pb/AnyMessage.h>

#include <fcpb/test/SimpleMessage.pb.h>
#include <fcpb/comm/Request.pb.h>

#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace fluffycoin;

namespace
{

struct TestContext
{
    async::TreasureTrove trove;
    async::Scheduler scheduler;
    svc::ServiceScene ctx;
    zmq::Context zmq;
    svc::ApiHandlerMap apiMap;
    TestContext()
        : ctx(
            [](svc::ServiceStatusCode) noexcept -> void {},
            scheduler.asio(),
            trove)
    {
        scheduler.start();
    }
};

}

TEST(ApiPort, SimpleStartStop)
{
    TestContext ctx;
    svc::ApiPort port(ctx.ctx, ctx.zmq, ctx.apiMap);

    constexpr const uint16_t PORT = 1337;

    EXPECT_TRUE(port.openPort(PORT));
}

TEST(ApiPort, HandleApiCall)
{
    TestContext ctx;

    // Register a handler
    std::mutex mtx;
    std::condition_variable cond;
    std::atomic<bool> handled = false;
    auto myHandler = [&handled, &mtx, &cond](
        svc::RequestScene &scene,
        fcpb::test::SimpleMessage &msg) mutable ->
            boost::asio::awaitable<std::unique_ptr<google::protobuf::Message>>
    {
        EXPECT_EQ(msg.int_value(), 123);
        EXPECT_EQ(msg.string_value(), "Ghw");
        EXPECT_TRUE(scene.isOk());

        handled = true;

        std::lock_guard<std::mutex> lock(mtx);
        cond.notify_one();
        co_return std::unique_ptr<google::protobuf::Message>();
    };
    ctx.apiMap.add<fcpb::test::SimpleMessage>(myHandler);

    // Open the port
    svc::ApiPort port(ctx.ctx, ctx.zmq, ctx.apiMap);
    constexpr const uint16_t PORT = 1337;
    ASSERT_TRUE(port.openPort(PORT));

    // Build message to send
    fcpb::test::SimpleMessage msg;
    msg.set_int_value(123);
    msg.set_string_value("Ghw");

    // Stick in request wrapper
    fcpb::comm::Request envelope;
    pb::AnyMessage::toAny(msg, *envelope.mutable_body());

    // Serialize
    Details details;
    std::string data;
    pb::Json::toJson(envelope, data, details);
    ASSERT_TRUE(details.isOk());
    EXPECT_FALSE(data.empty());

    // Connect to API port
    zmq::Client client(ctx.zmq);
    client.connect("127.0.0.1", PORT, details);
    ASSERT_TRUE(details.isOk());

    // Send message
    std::unique_lock<std::mutex> lock(mtx);
    client.send(data, details);
    ASSERT_TRUE(details.isOk());

    // Queue client response reader
    async::UncontrolledSocket clientSock(ctx.scheduler.asio(), client.getFd());
    ASSERT_TRUE(clientSock);
    BinData rspData;
    std::atomic<bool> rspHandled = false;
    std::function<void(Details &)> respHandler =
    [&rspData, &client, &clientSock, &respHandler, &mtx, &cond, &rspHandled](Details &details) mutable -> void
    {
        // Error state
        if (!details.isOk())
        {
            std::lock_guard<std::mutex> lock(mtx);
            cond.notify_one();
        }
        // Not error state
        else
        {
            // We have read a ZMQ message
            if (client.recv(rspData, details))
            {
                std::lock_guard<std::mutex> lock(mtx);
                rspHandled = true;
                cond.notify_one();
            }
            // Still waiting, requeue
            else if (details.isOk())
            {
                clientSock.read(details, respHandler);
            }
        }
    };
    clientSock.read(details, respHandler);

    // Wait for message to be handled
    cond.wait_for(lock, std::chrono::seconds(2));

    // Ensure message was handled
    ASSERT_TRUE(handled);

    // Wait for response (if we don't already have it)
    if (!rspHandled)
        cond.wait_for(lock, std::chrono::seconds(2));

    // Did we get the response?
    EXPECT_FALSE(rspData.empty());
}
