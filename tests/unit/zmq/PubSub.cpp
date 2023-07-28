#include "fluffytest.h"

#include <fluffycoin/zmq/Publisher.h>
#include <fluffycoin/zmq/Subscriber.h>
#include <fluffycoin/ossl/Curve25519.h>

#include <unistd.h>

using namespace fluffycoin;

namespace
{

void pubSubTest(
    const BinData &serverPub,
    const BinData &serverPriv)
{
    constexpr const char *LOCALHOST = "127.0.0.1";
    constexpr const uint16_t port = 1337;
    constexpr const char *SUBSCRIBED_TOPIC_1 = "topic1";
    constexpr const char *SUBSCRIBED_TOPIC_2 = "topic2";
    constexpr const char *UNSUBSCRIBED_TOPIC = "topic3";

    zmq::Context ctx;

    // Bind server port
    zmq::Publisher tServer(ctx);
    Details details;
    tServer.bind("", port, serverPriv, details);
    //tServer.bind(LOCALHOST, port, serverPriv, details);
    ASSERT_TRUE(details.isOk());

    // Connect client port
    zmq::Subscriber tClient(ctx);
    tClient.connect(LOCALHOST, port, serverPub, details);
    ASSERT_TRUE(details.isOk());

    // Subscribe to topics
    tClient.subscribe(SUBSCRIBED_TOPIC_1, details);
    ASSERT_TRUE(details.isOk());
    tClient.subscribe(SUBSCRIBED_TOPIC_2, details);
    ASSERT_TRUE(details.isOk());

    // Subscriber needs to be connected to get the event
    constexpr const unsigned int SUBSCRIBE_DELAY_MS = 20;
    usleep(SUBSCRIBE_DELAY_MS * 1000);

    // Publish topic 1
    BinData msgData("This event");
    tServer.publish(SUBSCRIBED_TOPIC_1, msgData, details);
    ASSERT_TRUE(details.isOk());

    // Receive topic on subscriber
    BinData eventData;
    constexpr const unsigned int RETRIES = 10;
    constexpr const unsigned int RETRY_SLEEP_MS = 1;
    unsigned int tries = 0;
    bool hasMsg = false;
    std::string topic;
    while (tries++ < RETRIES && !hasMsg)
    {
        hasMsg = tClient.recv(topic, eventData, details);
        EXPECT_TRUE(details.isOk());
        if (!hasMsg && tries <= RETRIES)
            usleep(RETRY_SLEEP_MS * 1000);
    }

    // Got event from server?
    ASSERT_TRUE(hasMsg);
    ASSERT_TRUE(details.isOk());
    EXPECT_EQ(msgData, eventData);
    EXPECT_EQ(topic, SUBSCRIBED_TOPIC_1);

    // Publish to topic 2
    msgData = BinData("Next event");
    tServer.publish(SUBSCRIBED_TOPIC_2, msgData, details);
    ASSERT_TRUE(details.isOk());

    // Receive reply on client socket
    eventData.clear();
    topic.clear();
    tries = 0;
    hasMsg = false;
    while (tries++ < RETRIES && !hasMsg)
    {
        hasMsg = tClient.recv(topic, eventData, details);
        EXPECT_TRUE(details.isOk());
        if (!hasMsg && tries <= RETRIES)
            usleep(RETRY_SLEEP_MS * 1000);
    }

    // Got sub2 from server?
    ASSERT_TRUE(hasMsg);
    ASSERT_TRUE(details.isOk());
    EXPECT_EQ(msgData, eventData);
    EXPECT_EQ(topic, SUBSCRIBED_TOPIC_2);

    // Publish to topic 3
    msgData = BinData("Next next event");
    tServer.publish(UNSUBSCRIBED_TOPIC, msgData, details);
    ASSERT_TRUE(details.isOk());

    // Receive reply on client socket
    topic.clear();
    eventData.clear();
    tries = 0;
    hasMsg = false;
    while (tries++ < RETRIES && !hasMsg)
    {
        hasMsg = tClient.recv(topic, eventData, details);
        EXPECT_TRUE(details.isOk());
        if (!hasMsg && tries <= RETRIES)
            usleep(RETRY_SLEEP_MS * 1000);
    }

    // Should not have received the event
    ASSERT_TRUE(details.isOk());
    EXPECT_FALSE(hasMsg);
    EXPECT_TRUE(topic.empty());
}

}

TEST(ZmqPubSub, EventTest)
{
    pubSubTest(BinData(), BinData());
}

TEST(ZmqPubSub, CurveMQ)
{
    // Generate server keypair
    ossl::EvpPkeyPtr serverPkey = ossl::Curve25519::generate();
    ASSERT_NE(serverPkey.get(), nullptr);

    // Serialize to binary
    BinData serverPubKey = ossl::Curve25519::toPublic(*serverPkey);
    SafeData serverPrivKey = ossl::Curve25519::toPrivate(*serverPkey);
    ASSERT_FALSE(serverPubKey.empty());
    ASSERT_FALSE(serverPrivKey.empty());

    pubSubTest(serverPubKey, serverPrivKey);
}
