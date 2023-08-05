#include <fluffycoin/alg/PeerAuth.h>

#include <fluffycoin/ossl/Hash.h>
#include <fluffycoin/ossl/Curve25519.h>

#include <fluffycoin/utils/Base64.h>
#include <fluffycoin/utils/Strings.h>

#include <fluffycoin/log/Log.h>

#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>

using namespace fluffycoin;
using namespace fluffycoin::alg;

namespace
{

constexpr const char *MESSAGE_AUTH_KDF_MGF = "SHA256";
constexpr size_t MESSAGE_AUTH_KDF_MGF_LEN = constStrlen(MESSAGE_AUTH_KDF_MGF);

constexpr const size_t MESSAGE_AUTH_KEY_LEN = 32;

}

SafeData PeerAuth::createMessageAuthKey(
    const EVP_PKEY &priv,
    const BinData &pub,
    const BinData &clientNonce,
    const BinData &serverNonce)
{
    SafeData hmacKey;

    // Deserialize peer key
    ossl::EvpPkeyPtr peerKey = ossl::Curve25519::fromPublic(pub);
    if (!peerKey)
    {
        log::error("Failed to convert peer public point to EVP key.");
        return hmacKey;
    }

    // Setup context
    ossl::EvpPkeyCtxPtr ctx(EVP_PKEY_CTX_new(const_cast<EVP_PKEY *>(&priv), nullptr));
    if (!ctx)
    {
        log::error("Failed to allocate EVP key context.");
        return hmacKey;
    }

    int ret = EVP_PKEY_derive_init(ctx.get());
    if (ret != 1)
    {
        log::error("Failed to initialie EVP key context.");
        return hmacKey;
    }

    ret = EVP_PKEY_derive_set_peer(ctx.get(), peerKey.get());
    if (ret != 1)
    {
        log::error("Failed to set peer key on EVP key context.");
        return hmacKey;
    }

    // Get shared secret length
    size_t secretLen = 0;
    ret = EVP_PKEY_derive(ctx.get(), nullptr, &secretLen);
    if (ret != 1 || !secretLen)
    {
        log::error("Failed to create shared secret.");
        return hmacKey;
    }

    // Derive shared secret
    SafeData sharedSecret;
    sharedSecret.resize(secretLen);
    ret = EVP_PKEY_derive(ctx.get(), sharedSecret.data(), &secretLen);
    if (ret != 1 || secretLen != sharedSecret.length())
    {
        log::error("Failed to create shared secret.");
        return hmacKey;
    }

    // Derive HMAC key from shared secret and session nonces

    // Initialise the KDF operation
    ossl::EvpPkeyCtxPtr kdfCtx(EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr));
    EVP_PKEY_derive_init(kdfCtx.get());

    // Salt is combined nonces
    BinData combinedNonce;
    combinedNonce += serverNonce;
    combinedNonce += clientNonce;

    // Set operation parameters
    OSSL_PARAM params[4] = {
        // MGF
        OSSL_PARAM_construct_utf8_string(
            OSSL_KDF_PARAM_DIGEST, const_cast<char *>(MESSAGE_AUTH_KDF_MGF), MESSAGE_AUTH_KDF_MGF_LEN),
        // Salt
        OSSL_PARAM_construct_octet_string(
            OSSL_KDF_PARAM_SALT, const_cast<unsigned char *>(serverNonce.data()), serverNonce.length()),
        // Secret
        OSSL_PARAM_construct_octet_string(
            OSSL_KDF_PARAM_KEY, sharedSecret.data(), sharedSecret.length()),

        OSSL_PARAM_construct_end()
    };
    EVP_PKEY_CTX_set_params(kdfCtx.get(), params);

    // NIST 800-56Cr1 key derivation
    hmacKey.resize(MESSAGE_AUTH_KEY_LEN);
    size_t keyLen = MESSAGE_AUTH_KEY_LEN;
    ret = EVP_PKEY_derive(kdfCtx.get(), hmacKey.data(), &keyLen);
    if (ret != 1 || keyLen != MESSAGE_AUTH_KEY_LEN)
    {
        hmacKey.clear();
        log::error("Failed to derive message authentication key.");
        return hmacKey;
    }

    return hmacKey;
}

std::string PeerAuth::createSessionId(
    const BinData &clientNonce,
    const BinData &serverNonce)
{
    // Base64(Truncate(SHA256(clientNonce | serverNonce), 8))
    BinData hash = ossl::Hash::sha2_256(clientNonce + serverNonce);
    return Base64::encode(hash.sub(0, 8));
}
