#include "fluffytest.h"

#include <fluffycoin/svc/EventProcessor.h>

#include <fluffycoin/async/Scheduler.h>

#include <fluffycoin/zmq/Publisher.h>

#include <fluffycoin/ossl/Curve25519.h>

#include <fluffycoin/pb/Json.h>
#include <fluffycoin/pb/Catalog.h>
#include <fluffycoin/pb/AnyMessage.h>

#include <fcpb/comm/Event.pb.h>
#include <fcpb/test/SimpleMessage.pb.h>

#include <boost/asio/io_context.hpp>

using namespace fluffycoin;

namespace
{

struct TestContext
{
    async::TreasureTrove trove;
    async::Scheduler scheduler;
    svc::ServiceScene ctx;
    zmq::Context zmq;
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

TEST(EventProcessor, Local)
{
    TestContext ctx;

    constexpr const uint16_t PORT = 1337;
    constexpr const char *TOPIC = "Test123";
    constexpr const char *LOCALHOST = "127.0.0.1";

    constexpr const int TEST_INT = 1'2'3'4;
    constexpr const char *TEST_STR = "1.2.3.4.5";

    // Start localhost clear publisher
    Details details;
    zmq::Publisher publisher(ctx.zmq);
    publisher.bind(LOCALHOST, PORT, details);
    EXPECT_TRUE(details.isOk());

    std::mutex mtx;
    std::condition_variable cond;
    std::atomic<bool> handled = false;

    // Set up subscription handler
    svc::EventSubscriptionMap subMap;
    subMap.addServiceSub<fcpb::test::SimpleMessage>(PORT, TOPIC,
        [&handled, &mtx, &cond, TEST_INT, TEST_STR](const svc::ServiceScene &ctx, fcpb::test::SimpleMessage &event) -> void
        {
            (void)ctx;

            EXPECT_FALSE(handled);
            EXPECT_EQ(event.int_value(), TEST_INT);
            EXPECT_EQ(event.string_value(), TEST_STR);
            handled = true;

            std::lock_guard<std::mutex> lock(mtx);
            cond.notify_one();
        });

    // Setup subscription processor
    svc::EventProcessor proc(ctx.ctx, ctx.zmq, subMap);
    EXPECT_TRUE(proc.subscribeLocal());

    // Subscriber needs to be connected to get the event
    constexpr const unsigned int SUBSCRIBE_DELAY_MS = 20;
    usleep(SUBSCRIBE_DELAY_MS * 1000);

    // Build event
    fcpb::test::SimpleMessage msg;
    msg.set_int_value(TEST_INT);
    msg.set_string_value(TEST_STR);
    // Put int envelope
    fcpb::comm::Event event;
    pb::AnyMessage::toAny(msg, *event.mutable_body());
    std::string value;
    pb::Json::toJson(event, value, details);
    EXPECT_TRUE(details.isOk());

    // Publish event
    std::unique_lock<std::mutex> lock(mtx);
    publisher.publish(TOPIC, value, details);
    EXPECT_TRUE(details.isOk());

    // Make sure we got it
    cond.wait_for(lock, std::chrono::seconds(1));
    EXPECT_TRUE(handled);
    lock.unlock();
}

TEST(EventProcessor, Peer)
{
    TestContext ctx;

    constexpr const uint16_t PORT = 1337;
    constexpr const char *TOPIC = "Test123";
    constexpr const char *LOCALHOST = "127.0.0.1";

    constexpr const int TEST_INT = 1'2'3'4;
    constexpr const char *TEST_STR = "1.2.3.4.5";

    // Generate server key
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key.get(), nullptr);

    SafeData privKey = ossl::Curve25519::toPrivate(*key);
    EXPECT_FALSE(privKey.empty());

    BinData pubKey = ossl::Curve25519::toPublic(*key);
    EXPECT_FALSE(pubKey.empty());

    // Start localhost "peer" publisher
    Details details;
    zmq::Publisher publisher(ctx.zmq);
    publisher.bind(LOCALHOST, PORT, privKey, details);
    EXPECT_TRUE(details.isOk());

    std::mutex mtx;
    std::condition_variable cond;
    std::atomic<bool> handled = false;

    // Set up subscription handler
    svc::EventSubscriptionMap subMap;
    subMap.addPeerSub<fcpb::test::SimpleMessage>(TOPIC,
        [&handled, &mtx, &cond, TEST_INT, TEST_STR](const svc::ServiceScene &ctx, fcpb::test::SimpleMessage &event) -> void
        {
            (void)ctx;

            EXPECT_FALSE(handled);
            EXPECT_EQ(event.int_value(), TEST_INT);
            EXPECT_EQ(event.string_value(), TEST_STR);
            handled = true;

            std::lock_guard<std::mutex> lock(mtx);
            cond.notify_one();
        });

    // Setup subscription processor
    svc::EventProcessor proc(ctx.ctx, ctx.zmq, subMap);
    EXPECT_TRUE(proc.addPeer(pubKey, LOCALHOST, PORT));

    // Subscriber needs to be connected to get the event
    constexpr const unsigned int SUBSCRIBE_DELAY_MS = 20;
    usleep(SUBSCRIBE_DELAY_MS * 1000);

    // Build event
    fcpb::test::SimpleMessage msg;
    msg.set_int_value(TEST_INT);
    msg.set_string_value(TEST_STR);
    // Put int envelope
    fcpb::comm::Event event;
    pb::AnyMessage::toAny(msg, *event.mutable_body());
    std::string value;
    pb::Json::toJson(event, value, details);
    EXPECT_TRUE(details.isOk());

    // Publish event
    std::unique_lock<std::mutex> lock(mtx);
    publisher.publish(TOPIC, value, details);
    EXPECT_TRUE(details.isOk());

    // Make sure we got it
    cond.wait_for(lock, std::chrono::seconds(1));
    EXPECT_TRUE(handled);
    lock.unlock();
}
