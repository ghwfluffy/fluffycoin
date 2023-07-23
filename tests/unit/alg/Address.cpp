#include "fluffytest.h"

#include <fluffycoin/alg/info.h>
#include <fluffycoin/alg/Address.h>

#include <fluffycoin/ossl/Curve25519.h>

using namespace fluffycoin;

TEST(Address, Generate)
{
    // Make public key from static data
    unsigned char pubkeyData[ossl::Curve25519::POINT_LEN] = {};
    ossl::EvpPkeyPtr key = ossl::Curve25519::fromPublic(BinData(pubkeyData, sizeof(pubkeyData)));
    ASSERT_NE(key, nullptr);

    // Encode address with static seed
    BinData address = alg::Address::generate(*key, "DEADBEEF");
    EXPECT_FALSE(address.empty());

    // To printable
    std::string printableAddress = alg::Address::printable(address);
    EXPECT_EQ(printableAddress, "DEADBEEFHLEvfWllFyBMLpeYw24obZ_dNmU");

    // From printable
    BinData decodedAddress = alg::Address::unprintable(printableAddress);
    EXPECT_EQ(address, decodedAddress);

    // Verify
    EXPECT_TRUE(alg::Address::verify(decodedAddress, *key));
}
