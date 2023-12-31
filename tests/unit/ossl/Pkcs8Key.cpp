#include "fluffytest.h"

#include <fluffycoin/ossl/Curve25519.h>
#include <fluffycoin/ossl/Pkcs8Key.h>

using namespace fluffycoin;

TEST(Pkcs8Key, EncodeDecode)
{
    // Generate keypair
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key.get(), nullptr);

    constexpr const char *PASSWORD = "ghw";
    constexpr const unsigned int KDF_ITERS = 1024;

    // Encrypt key for storage
    BinData encoded = ossl::Pkcs8Key::wrap(*key, PASSWORD, KDF_ITERS);
    EXPECT_GT(encoded.length(), 0);

    // Decrypt and decode key
    ossl::EvpPkeyPtr dupKey = ossl::Pkcs8Key::unwrap(encoded, PASSWORD);
    EXPECT_NE(dupKey.get(), nullptr);

    // Sign with original
    BinData data(std::string("SIGN ME"));
    BinData signature = ossl::Curve25519::sign(*key, data);
    EXPECT_EQ(signature.length(), 64);

    // Verify with duplicate
    bool verified = ossl::Curve25519::verify(*dupKey, data, signature);
    EXPECT_TRUE(verified);
}
