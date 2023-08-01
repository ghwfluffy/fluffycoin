#include "fluffytest.h"

#include <fluffycoin/svc/EventPublisher.h>

#include <fluffycoin/zmq/Subscriber.h>

#include <fluffycoin/ossl/Curve25519.h>

#include <fluffycoin/pb/Json.h>
#include <fluffycoin/pb/Catalog.h>
#include <fluffycoin/pb/AnyMessage.h>

#include <fcpb/comm/Event.pb.h>
#include <fcpb/test/SimpleMessage.pb.h>

using namespace fluffycoin;

TEST(EventPublisher, SimpleStartStop)
{
    zmq::Context zmq;
    svc::EventPublisher publisher(zmq);
    constexpr const uint16_t PORT = 1337;
    EXPECT_TRUE(publisher.openPort(PORT));
}

TEST(EventPublisher, SimpleStartStopCurveMQ)
{
    zmq::Context zmq;
    svc::EventPublisher publisher(zmq);

    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key.get(), nullptr);

    SafeData priv = ossl::Curve25519::toPrivate(*key);
    EXPECT_FALSE(priv.empty());

    constexpr const uint16_t PORT = 1337;
    EXPECT_TRUE(publisher.openPort(PORT, priv));
}

namespace
{

void subscriberTest(
    const BinData &serverPriv,
    const BinData &serverPub)
{
    zmq::Context zmq;
    svc::EventPublisher publisher(zmq);

    // Open subscription port
    constexpr const uint16_t PORT = 1337;
    EXPECT_TRUE(publisher.openPort(PORT, serverPriv));

    // Connect subscriber
    Details details;
    zmq::Subscriber subscriber(zmq);
    subscriber.connect("127.0.0.1", PORT, serverPub, details);
    EXPECT_TRUE(details.isOk());

    // Subscriber needs to be connected to get the event
    constexpr const unsigned int SUBSCRIBE_DELAY_MS = 20;
    usleep(SUBSCRIBE_DELAY_MS * 1000);

    constexpr const char *TOPIC = "Fluffycoin";
    subscriber.subscribeAll(details);
    EXPECT_TRUE(details.isOk());

    constexpr const int TEST_INT = 111;
    constexpr const char *TEST_STR = "event";
    fcpb::test::SimpleMessage msg;
    msg.set_int_value(TEST_INT);
    msg.set_string_value(TEST_STR);

    // Publish an event
    publisher.publish(TOPIC, msg);

    constexpr const unsigned int RETRIES = 10;
    constexpr const unsigned int RETRY_SLEEP_MS = 1;

    // Receive event
    BinData binMsg;
    std::string topic;
    bool hasMsg = false;
    unsigned int tries = 0;
    while (tries++ < RETRIES && !hasMsg)
    {
        hasMsg = subscriber.recv(topic, binMsg, details);
        EXPECT_TRUE(details.isOk());
        if (!hasMsg && tries <= RETRIES)
            usleep(RETRY_SLEEP_MS * 1000);
    }
    EXPECT_TRUE(hasMsg);
    EXPECT_FALSE(binMsg.empty());
    EXPECT_EQ(topic, TOPIC);

    // Decode into event
    fcpb::comm::Event envelope;
    pb::Json::fromJson(binMsg.data(), binMsg.length(), envelope, details);
    EXPECT_TRUE(details.isOk());

    // Retrieve simple message from event
    pb::Catalog::registerMsg<fcpb::test::SimpleMessage>();
    std::unique_ptr<google::protobuf::Message> body;
    pb::AnyMessage::fromAny(envelope.body(), body, details);
    EXPECT_TRUE(details.isOk());
    ASSERT_NE(body.get(), nullptr);
    ASSERT_NE(dynamic_cast<fcpb::test::SimpleMessage *>(body.get()), nullptr);

    // Deserialized values match
    fcpb::test::SimpleMessage &inner = *dynamic_cast<fcpb::test::SimpleMessage *>(body.get());
    EXPECT_EQ(inner.int_value(), TEST_INT);
    EXPECT_EQ(inner.string_value(), TEST_STR);
}

}

TEST(EventPublisher, Subscriber)
{
    subscriberTest(BinData(), BinData());
}

TEST(EventPublisher, SubscriberCurveMQ)
{
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key.get(), nullptr);

    SafeData priv = ossl::Curve25519::toPrivate(*key);
    EXPECT_FALSE(priv.empty());

    BinData pub = ossl::Curve25519::toPublic(*key);
    EXPECT_FALSE(pub.empty());

    subscriberTest(priv, pub);
}
