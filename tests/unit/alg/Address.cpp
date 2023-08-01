#include "fluffytest.h"

#include <fluffycoin/alg/info.h>
#include <fluffycoin/alg/Address.h>

#include <fluffycoin/ossl/Curve25519.h>

using namespace fluffycoin;

TEST(Address, Static)
{
    // Make public key from static data
    unsigned char pubkeyData[ossl::Curve25519::POINT_LEN] = {};
    ossl::EvpPkeyPtr key = ossl::Curve25519::fromPublic(BinData(pubkeyData, sizeof(pubkeyData)));
    ASSERT_NE(key, nullptr);

    // Encode address with static salt
    BinData address = alg::Address::generate(*key, "DEADBEEF");
    EXPECT_FALSE(address.empty());
    EXPECT_TRUE(alg::Address::verify(address, *key));

    // To printable
    std::string printableAddress = alg::Address::printable(address);
    EXPECT_EQ(printableAddress, "DEADBEEFQBh3WpP9XJ87narxiqSbCXw6xcL");

    // From printable
    BinData decodedAddress = alg::Address::unprintable(printableAddress);
    EXPECT_EQ(address, decodedAddress);

    // Verify
    EXPECT_TRUE(alg::Address::verify(decodedAddress, *key));
}

TEST(Address, Random)
{
    // Make random private key
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key, nullptr);

    // Encode address with random salt
    BinData address = alg::Address::generate(*key);
    EXPECT_FALSE(address.empty());
    EXPECT_TRUE(alg::Address::verify(address, *key));

    // To printable
    std::string printableAddress = alg::Address::printable(address);
    EXPECT_FALSE(printableAddress.empty());

    // From printable
    BinData decodedAddress = alg::Address::unprintable(printableAddress);
    EXPECT_EQ(address, decodedAddress);

    // Verify
    EXPECT_TRUE(alg::Address::verify(decodedAddress, *key));
}
