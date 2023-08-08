#include "fluffytest.h"

#include <fluffycoin/alg/Wallet.h>
#include <fluffycoin/alg/Address.h>
#include <fluffycoin/ossl/Curve25519.h>

using namespace fluffycoin;

TEST(Wallet, Success)
{
    alg::Wallet wallet;
    wallet.setEncFormat(alg::Wallet::EncFormat::FcPbkdf, 1024);

    // Add new key directly to wallet
    alg::Wallet::KeyUsage usage = alg::Wallet::KeyUsage::Specie;
    ossl::EvpPkeyPtr key = wallet.makeNewKey(usage, "GHW");
    ASSERT_NE(key, nullptr);
    EXPECT_EQ(wallet.getLatestAddress(usage).substr(0, 3), "GHW");

    // Make random private key
    key = ossl::Curve25519::generate();
    ASSERT_NE(key, nullptr);
    std::string newAddress = alg::Address::printable(alg::Address::generate(*key));
    wallet.addKey(usage, *key, newAddress);

    // Expect two keys
    EXPECT_EQ(wallet.getKeys().size(), 2);

    // Convert to string
    std::string pw = "password";
    std::string serialized = wallet.getString(pw);
    EXPECT_FALSE(serialized.empty());

    // Deserialize
    wallet = alg::Wallet();
    wallet.setEncFormat(alg::Wallet::EncFormat::FcPbkdf, 1024);
    bool bOk = wallet.setString(serialized, pw);
    EXPECT_TRUE(bOk);

    // Expect two keys
    EXPECT_EQ(wallet.getKeys().size(), 2);

    // Expect latest key to exist
    EXPECT_NE(wallet.getLatestKey(usage).get(), nullptr);
    EXPECT_EQ(wallet.getLatestAddress(usage), newAddress);
}
