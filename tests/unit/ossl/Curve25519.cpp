#include "fluffytest.h"

#include <fluffycoin/ossl/Curve25519.h>
#include <fluffycoin/ossl/Prng.h>

#include <fluffycoin/utils/Hex.h>

#include <fluffycoin/log/Log.h>

using namespace fluffycoin;

TEST(Curve25519, GenerateKeyPair)
{
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    EXPECT_NE(key.get(), nullptr);
}

TEST(Curve25519, EncodeKey)
{
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key.get(), nullptr);

    BinData pubkey = ossl::Curve25519::toPublic(*key);
    EXPECT_EQ(pubkey.length(), 32);
    log::traffic("Public key: {}",  Hex::encode(pubkey));

    ossl::EvpPkeyPtr pub = ossl::Curve25519::fromPublic(pubkey);
    EXPECT_NE(pub.get(), nullptr);

    SafeData privkey = ossl::Curve25519::toPrivate(*key);
    EXPECT_EQ(privkey.length(), pubkey.length());
    log::traffic("Private key: {}",  Hex::encode(privkey));

    ossl::EvpPkeyPtr priv = ossl::Curve25519::fromPrivate(privkey);
    EXPECT_NE(priv.get(), nullptr);
}

TEST(Curve25519, SignVerify)
{
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key.get(), nullptr);

    BinData data(std::string("SIGN ME"));
    BinData signature = ossl::Curve25519::sign(*key, data);
    EXPECT_EQ(signature.length(), 64);

    bool verified = ossl::Curve25519::verify(*key, data, signature);
    EXPECT_TRUE(verified);
}

TEST(Curve25519, SignNoVerify)
{
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key.get(), nullptr);

    BinData data(std::string("SIGN ME"));
    BinData signature = ossl::Curve25519::sign(*key, data);
    ASSERT_EQ(signature.length(), 64);

    BinData badSig = signature;
    badSig.data()[static_cast<size_t>(ossl::Prng::randInt(0, 63))] =
        static_cast<unsigned char>(ossl::Prng::randInt(0, 255));
    bool verified = ossl::Curve25519::verify(*key, data, badSig);
    EXPECT_FALSE(verified);

    verified = ossl::Curve25519::verify(*key, "SIGN MEE", signature);
    EXPECT_FALSE(verified);
}

TEST(Curve25519, EncodeDecodeSignVerify)
{
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key.get(), nullptr);

    BinData pubkey = ossl::Curve25519::toPublic(*key);
    EXPECT_EQ(pubkey.length(), 32);

    ossl::EvpPkeyPtr pub = ossl::Curve25519::fromPublic(pubkey);
    EXPECT_NE(pub.get(), nullptr);

    SafeData privkey = ossl::Curve25519::toPrivate(*key);
    EXPECT_EQ(privkey.length(), pubkey.length());

    ossl::EvpPkeyPtr priv = ossl::Curve25519::fromPrivate(privkey);
    EXPECT_NE(priv.get(), nullptr);

    BinData data(std::string("SIGN ME"));
    BinData signature = ossl::Curve25519::sign(*priv, data);
    EXPECT_EQ(signature.length(), 64);

    bool verified = ossl::Curve25519::verify(*pub, data, signature);
    EXPECT_TRUE(verified);
}
