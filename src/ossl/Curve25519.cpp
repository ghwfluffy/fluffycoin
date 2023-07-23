#include <fluffycoin/ossl/Curve25519.h>
#include <fluffycoin/ossl/encode.h>
#include <fluffycoin/ossl/decode.h>
#include <fluffycoin/ossl/Error.h>

#include <fluffycoin/log/Log.h>

#include <openssl/evp.h>
#include <openssl/x509.h>

#include <functional>

using namespace fluffycoin;
using namespace fluffycoin::ossl;

namespace
{

// We're going to piggy back off the SubjectPublicKeyInfo format
// for en/decoding public key points
constexpr const unsigned char pubinfo[] =
{
    0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x03, 0x21, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

constexpr const size_t PUB_ENCODING_LEN = sizeof(pubinfo) - Curve25519::POINT_LEN;

// EVP_PKEY serialized ED25519 private key
constexpr const unsigned char privinfo[] =
{
    0x30, 0x2e, 0x02, 0x01, 0x00, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70,
    0x04, 0x22, 0x04, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

constexpr const size_t PRIV_ENCODING_LEN = sizeof(privinfo) - Curve25519::POINT_LEN;

}

EvpPkeyPtr Curve25519::generate()
{
    // Generate keypair
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY *key = nullptr;
    int ret = EVP_PKEY_keygen(ctx, &key);
    EVP_PKEY_CTX_free(ctx);

    EvpPkeyPtr retKey;
    if (ret != 1 || !key)
        log::error("Failed to generate ED25519 keypair: {}", ossl::Error::pop());
    else
        retKey = EvpPkeyPtr(key);

    return retKey;
}

BinData Curve25519::toPublic(const EVP_PKEY &key)
{
    BinData ret;

    // Encode SubjectPublicKeyInfo
    BinData x509 = ossl::encode(key, i2d_PUBKEY);

    // Verify they really gave us an ED25519 key
    if (x509.length() != sizeof(pubinfo) || memcmp(pubinfo, x509.data(), PUB_ENCODING_LEN) != 0)
        log::error("Failed to encode ED25519 public key.");
    // Strip the encoding
    else
        ret.setData(x509.data() + PUB_ENCODING_LEN, POINT_LEN);

    return ret;
}

EvpPkeyPtr Curve25519::fromPublic(const BinData &publicPoint)
{
    EvpPkeyPtr retKey;

    if (publicPoint.length() != POINT_LEN)
        log::error("ED25519 public point invalid length.");
    else
    {
        unsigned char encoded[sizeof(pubinfo)];
        memcpy(encoded, pubinfo, sizeof(pubinfo));
        memcpy(encoded + PUB_ENCODING_LEN, publicPoint.data(), POINT_LEN);

        const unsigned char *pauc = encoded;
        EVP_PKEY *key = nullptr;
        d2i_PUBKEY(&key, &pauc, static_cast<long>(sizeof(encoded)));
        if (!key)
            log::error("Failed to decode ED25519 public point: {}", ossl::Error::pop());
        else
            retKey.reset(key);
    }

    return retKey;
}

SafeData Curve25519::toPrivate(const EVP_PKEY &key)
{
    SafeData ret = ossl::encode<SafeData>(key, i2d_PrivateKey);
    if (ret.length() < POINT_LEN)
    {
        log::error("Invalid length serialized private key {}.", ret.length());
        ret.clear();
    }
    else
    {
        ret.truncateFront(ret.length() - POINT_LEN);
    }

    return ret;
}

EvpPkeyPtr Curve25519::fromPrivate(const BinData &priv)
{
    EvpPkeyPtr ret;

    if (priv.length() != POINT_LEN)
        log::error("Invalid private key point length {}.", priv.length());
    else
    {
        SafeData encodedPriv(privinfo, sizeof(privinfo));
        memcpy(encodedPriv.data() + PRIV_ENCODING_LEN, priv.data(), priv.length());

        ret = ossl::decode<EvpPkeyPtr>(encodedPriv,
            std::bind(
                d2i_PrivateKey, EVP_PKEY_ED25519,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    return ret;
}

BinData Curve25519::privateToPublic(const BinData &priv)
{
    BinData ret;
    EvpPkeyPtr key = fromPrivate(priv);
    if (key)
        ret = toPublic(*key);
    return ret;
}

BinData Curve25519::sign(const EVP_PKEY &key, const BinData &data)
{
    bool ok = true;
    int len = EVP_PKEY_size(&key);
    if (len <= 0)
        ok = log::error("Failed to calculate signature size.");

    BinData signature;
    if (ok)
    {
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, const_cast<EVP_PKEY *>(&key));
        signature.resize(static_cast<size_t>(len));
        size_t sigLen = signature.length();
        int ret = EVP_DigestSign(ctx, signature.data(), &sigLen, data.data(), data.length());
        if (ret != 1)
        {
            signature.clear();
            ok = log::error("Failed to generate signature: {}", ossl::Error::pop());
        }
        else if (static_cast<size_t>(sigLen) != signature.length())
        {
            signature.clear();
            ok = log::error("Generated invalid signature size {}.", sigLen);
        }

        EVP_MD_CTX_free(ctx);
    }

    return signature;
}

bool Curve25519::verify(const EVP_PKEY &key, const BinData &data, const BinData &signature)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, const_cast<EVP_PKEY *>(&key));
    int ret = EVP_DigestVerify(
        ctx,
        signature.data(),
        signature.length(),
        data.data(),
        data.length());
    bool verified = ret == 1;
    if (ret < 0)
        log::error("Error during signature verification: {}", ossl::Error::pop());
    EVP_MD_CTX_free(ctx);

    return verified;
}
