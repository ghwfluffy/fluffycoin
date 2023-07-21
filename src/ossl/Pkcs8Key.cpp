#include <fluffycoin/ossl/Pkcs8Key.h>
#include <fluffycoin/ossl/encode.h>
#include <fluffycoin/ossl/Prng.h>

#include <fluffycoin/utils/SafeData.h>

#include <fluffycoin/log/Log.h>

#include <openssl/pkcs12.h>

using namespace fluffycoin;
using namespace fluffycoin::ossl;

namespace
{

constexpr const size_t SALT_LEN = 8;
constexpr const uint32_t KDF_ITERS = 1024 * 1024;

}

BinData Pkcs8Key::wrap(const EVP_PKEY &key, const BinData &pass)
{
    bool ok = true;

    // Make internal struct
    PKCS8_PRIV_KEY_INFO *bag = EVP_PKEY2PKCS8(&key);
    if (!bag)
        ok = log::error("Failed to convert key to PKCS#8 struct.");

    // Encrypt
    X509_SIG *wrapped = nullptr;
    if (ok)
    {
        // ECDSA
        PKCS8_add_keyusage(bag, KEY_SIG);

        // Generate random salt
        unsigned char salt[SALT_LEN];
        Prng::rand(salt, sizeof(salt));

        // PBE
        wrapped = PKCS8_encrypt(
            -1, // Default PBE (should be PBES2)
            EVP_aes_256_cbc(),
            reinterpret_cast<const char *>(pass.data()),
            static_cast<int>(pass.length()),
            salt,
            sizeof(salt),
            KDF_ITERS,
            bag);

        if (!wrapped)
            ok = log::error("Failed to encrypt PKCS#8 key.");
    }

    // Encode
    SafeData encoded;
    if (ok)
    {
        encoded = ossl::encode<SafeData>(*wrapped, i2d_X509_SIG);
        if (encoded.empty())
            ok = log::error("Failed to encode PKCS#8 key.");
    }

    // Cleanup
    PKCS8_PRIV_KEY_INFO_free(bag);

    return encoded;
}

EvpPkeyPtr Pkcs8Key::unwrap(const BinData &key, const BinData &pass)
{
    bool ok = true;

    // Decode
    const unsigned char *p = key.data();
    X509_SIG *encrypted = d2i_X509_SIG(nullptr, &p, static_cast<long>(key.length()));
    if (!encrypted)
        ok = log::error("Failed to decode PKCS#8 key.");

    // Decrypt
    PKCS8_PRIV_KEY_INFO *bag = nullptr;
    if (ok)
    {
        bag = PKCS8_decrypt(
            encrypted,
            reinterpret_cast<const char *>(pass.data()),
            static_cast<int>(pass.length()));
        if (!bag)
            ok = log::error("Failed to decrypt PKCS#8 key.");
    }

    // Get EVP Key
    EvpPkeyPtr ret;
    if (ok)
    {
        ret.reset(EVP_PKCS82PKEY(bag));
        if (!ret)
            ok = log::error("Failed to retrieve private key from PKCS#8 structure.");
    }

    // Cleanup
    X509_SIG_free(encrypted);
    PKCS8_PRIV_KEY_INFO_free(bag);

    return ret;
}
