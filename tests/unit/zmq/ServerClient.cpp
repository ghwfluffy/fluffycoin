#include "fluffytest.h"

#include <fluffycoin/zmq/Server.h>
#include <fluffycoin/zmq/Client.h>
#include <fluffycoin/ossl/Curve25519.h>

#include <unistd.h>

using namespace fluffycoin;

namespace
{

void clientServerTest(
    const BinData &serverPub,
    const BinData &serverPriv)
{
    constexpr const char *LOCALHOST = "127.0.0.1";
    constexpr const uint16_t port = 1337;

    zmq::Context ctx;

    // Bind server port
    zmq::Server tServer(ctx);
    Details details;
    tServer.bind(LOCALHOST, port, serverPriv, details);
    ASSERT_TRUE(details.isOk());

    // Connect client port
    zmq::Client tClient(ctx);
    tClient.connect(LOCALHOST, port, serverPub, details);
    ASSERT_TRUE(details.isOk());

    // Send message from client
    BinData msgData("This test");
    tClient.send(msgData, details);
    ASSERT_TRUE(details.isOk());

    // Receive message on server
    BinData serverMsgData;
    constexpr const unsigned int RETRIES = 3;
    constexpr const unsigned int RETRY_SLEEP_MS = 1;
    unsigned int tries = 0;
    bool hasMsg = false;
    zmq::ClientId clientId;
    while (tries++ < RETRIES && !hasMsg)
    {
        hasMsg = tServer.recv(clientId, serverMsgData, details);
        if (!hasMsg && tries <= RETRIES)
            usleep(RETRY_SLEEP_MS * 1000);
    }

    // Got message from client?
    ASSERT_TRUE(hasMsg);
    ASSERT_TRUE(details.isOk());
    EXPECT_EQ(msgData, serverMsgData);
    EXPECT_FALSE(clientId.empty());

    // Send something back to the client
    serverMsgData = BinData("Response Text");
    tServer.reply(clientId, serverMsgData, details);
    EXPECT_TRUE(details.isOk());

    // Receive reply on client socket
    msgData.clear();
    tries = 0;
    hasMsg = false;
    while (tries++ < RETRIES && !hasMsg)
    {
        hasMsg = tClient.recv(msgData, details);
        if (!hasMsg && tries <= RETRIES)
            usleep(RETRY_SLEEP_MS * 1000);
    }
    ASSERT_TRUE(hasMsg);
    ASSERT_TRUE(details.isOk());
    EXPECT_EQ(msgData, serverMsgData);
}

}

TEST(ZmqServerClient, SendMessage)
{
    clientServerTest(BinData(), BinData());
}

TEST(ZmqServerClient, CurveMQ)
{
    // Generate server keypair
    ossl::EvpPkeyPtr serverPkey = ossl::Curve25519::generate();
    ASSERT_NE(serverPkey.get(), nullptr);

    // Serialize to binary
    BinData serverPubKey = ossl::Curve25519::toPublic(*serverPkey);
    SafeData serverPrivKey = ossl::Curve25519::toPrivate(*serverPkey);
    ASSERT_FALSE(serverPubKey.empty());
    ASSERT_FALSE(serverPrivKey.empty());

    clientServerTest(serverPubKey, serverPrivKey);
}
