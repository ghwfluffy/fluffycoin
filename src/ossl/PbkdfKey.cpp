#include <fluffycoin/ossl/PbkdfKey.h>
#include <fluffycoin/ossl/Curve25519.h>
#include <fluffycoin/ossl/convert.h>
#include <fluffycoin/ossl/decode.h>
#include <fluffycoin/ossl/encode.h>
#include <fluffycoin/ossl/Error.h>
#include <fluffycoin/ossl/Prng.h>

#include <fluffycoin/utils/SafeData.h>

#include <fluffycoin/log/Log.h>

#include <openssl/aes.h>
#include <openssl/asn1t.h>

using namespace fluffycoin;
using namespace fluffycoin::ossl;

namespace
{

// Hardcoded parameters
constexpr const size_t SALT_LEN = 8;
constexpr const uint32_t KDF_ITERS = 1024 * 1024;
constexpr const int KDF_NID = NID_sha3_512;
constexpr const int KEY_NID = NID_aes_256_cbc;
constexpr const size_t IV_LEN = AES_BLOCK_SIZE;

constexpr const size_t KDF_ITERS_MIN = 1024;
constexpr const size_t KDF_ITERS_MAX = 10 * 1024 * 1024;
constexpr const size_t SALT_LEN_MIN = 8;
constexpr const size_t SALT_LEN_MAX = 1024;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

// PKCS8000 :p
// ya i probably should have just used PKCS8
typedef struct FluffyPbkdfInfo_st
{
    ASN1_INTEGER *version; // 0
    ASN1_INTEGER *iters; // 1024..10485760
    ASN1_OCTET_STRING *salt; // SIZE (8..1024)
    ASN1_OCTET_STRING *iv; // SIZE (0..16)
    ASN1_OBJECT *kdf;
    ASN1_OBJECT *keytype;
} FluffyPbkdfInfo;

DECLARE_ASN1_FUNCTIONS(FluffyPbkdfInfo)
IMPLEMENT_ASN1_FUNCTIONS(FluffyPbkdfInfo)

ASN1_SEQUENCE(FluffyPbkdfInfo) =
{
    ASN1_SIMPLE(FluffyPbkdfInfo, version, ASN1_INTEGER)
  , ASN1_SIMPLE(FluffyPbkdfInfo, iters, ASN1_INTEGER)
  , ASN1_SIMPLE(FluffyPbkdfInfo, salt, ASN1_OCTET_STRING)
  , ASN1_OPT(FluffyPbkdfInfo, iv, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(FluffyPbkdfInfo, kdf, ASN1_OBJECT)
  , ASN1_SIMPLE(FluffyPbkdfInfo, keytype, ASN1_OBJECT)
} ASN1_SEQUENCE_END(FluffyPbkdfInfo)

typedef struct FluffyPbkdfContent_st
{
    FluffyPbkdfInfo *info;
    ASN1_OCTET_STRING *payload;
} FluffyPbkdfContent;

DECLARE_ASN1_FUNCTIONS(FluffyPbkdfContent)
IMPLEMENT_ASN1_FUNCTIONS(FluffyPbkdfContent)

ASN1_SEQUENCE(FluffyPbkdfContent)
{
    ASN1_SIMPLE(FluffyPbkdfContent, info, FluffyPbkdfInfo)
  , ASN1_SIMPLE(FluffyPbkdfContent, payload, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(FluffyPbkdfContent)

typedef struct FluffyPbkdfKey_st
{
    FluffyPbkdfContent *content;
    ASN1_OCTET_STRING *signature;
} FluffyPbkdfKey;

DECLARE_ASN1_FUNCTIONS(FluffyPbkdfKey)
IMPLEMENT_ASN1_FUNCTIONS(FluffyPbkdfKey)
FLUFFYCOIN_DEFINE_UPTR(FluffyPbkdfKey)

ASN1_SEQUENCE(FluffyPbkdfKey)
{
    ASN1_SIMPLE(FluffyPbkdfKey, content, FluffyPbkdfContent)
  , ASN1_SIMPLE(FluffyPbkdfKey, signature, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(FluffyPbkdfKey)

#pragma GCC diagnostic pop

bool pbkdf(
    size_t keyLen,
    const BinData &pass,
    const BinData &salt,
    int iters,
    const EVP_MD *md,
    BinData &dek,
    BinData &mak)
{
    bool ok = true;

    SafeData keys;
    keys.resize(keyLen * 2);

    int ret = PKCS5_PBKDF2_HMAC(
        reinterpret_cast<const char *>(pass.data()),
        static_cast<int>(pass.length()),
        salt.data(),
        static_cast<int>(salt.length()),
        static_cast<int>(iters),
        md,
        static_cast<int>(keyLen * 2),
        keys.data());

    if (ret != 1)
        ok = log::error("Failed to generate key PBKDF keys.");
    else
    {
        dek.setData(keys.data(), keyLen);
        mak.setData(keys.data() + keyLen, keyLen);
    }

    return ok;
}

BinData sign(
    const BinData &content,
    const EVP_CIPHER *cipher,
    BinData &mak)
{
    // Get MAC algorithm info
    bool ok = true;
    EVP_MAC *mac = EVP_MAC_fetch(nullptr, "cmac", nullptr);
    if (!mac)
        ok = log::error("Failed to retrieve CMAC provider.");

    // Setup MAC context
    EVP_MAC_CTX *ctx = nullptr;
    if (ok)
    {
        const char *cipherName = EVP_CIPHER_get0_name(cipher);

        ctx = EVP_MAC_CTX_new(mac);
        OSSL_PARAM params[2] =
        {
            {
                "cipher",
                OSSL_PARAM_UTF8_STRING,
                const_cast<void *>(reinterpret_cast<const void *>(cipherName)),
                cipherName ? strlen(cipherName) : 0,
                0
            },
            OSSL_PARAM_construct_end(),
        };

        int ret = EVP_MAC_init(ctx, mak.data(), mak.length(), params);
        if (ret != 1)
            ok = log::error("Failed to initialize CMAC context for cipher {}.", EVP_CIPHER_get0_name(cipher));
    }

    // Generate signature
    BinData signature;
    if (ok)
    {
        EVP_MAC_update(ctx, content.data(), content.length());
        size_t siglen = EVP_MAC_CTX_get_mac_size(ctx);
        signature.resize(siglen);
        int ret = EVP_MAC_final(ctx, signature.data(), &siglen, signature.length());
        if (ret <= 0 || siglen != signature.length())
        {
            signature.clear();
            log::error("Failed to generate PBKDF key signature.");
        }

        EVP_MAC_CTX_free(ctx);
    }

    EVP_MAC_free(mac);

    return signature;
}

}

BinData PbkdfKey::wrap(const EVP_PKEY &key, const BinData &pass)
{
    bool ok = true;

    // Sane validate input
    if (pass.empty())
        ok = log::error("Null password provided to key PBKDF.");

    // Encode the private key
    SafeData encoded;
    if (ok)
    {
        encoded = Curve25519::toPrivate(key);
        if (encoded.empty())
            ok = log::error("Failed to encode private key for key PBKDF.");
    }

    // Generate salt and IV
    unsigned char iv[IV_LEN];
    unsigned char salt[SALT_LEN];
    if (ok)
    {
        Prng::rand(iv, sizeof(iv));
        Prng::rand(salt, sizeof(salt));
    }

    // Generate 2x AES-256 keys from password
    SafeData dek;
    SafeData mak;
    const EVP_CIPHER *cipher = nullptr;
    if (ok)
    {
        cipher = EVP_get_cipherbynid(KEY_NID);
        ok = pbkdf(
            static_cast<size_t>(EVP_CIPHER_get_key_length(cipher)),
            pass,
            BinData(salt, sizeof(salt)),
            static_cast<int>(KDF_ITERS),
            EVP_get_digestbynid(KDF_NID),
            dek,
            mak);
    }

    // Encipher
    BinData encipheredKey;
    if (ok)
    {
        int padLen = 0;
        int encipheredLen = 0;
        encipheredKey.resize(((encoded.length() + 16) / 16) * 16);
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, cipher, nullptr, dek.data(), iv);
        EVP_EncryptUpdate(ctx, encipheredKey.data(), &encipheredLen, encoded.data(), static_cast<int>(encoded.length()));
        EVP_EncryptFinal_ex(ctx, encipheredKey.data() + encipheredLen, &padLen);
        EVP_CIPHER_CTX_free(ctx);

        encipheredKey.resize(static_cast<size_t>(encipheredLen + padLen));
    }

    // Encode content
    BinData content;
    FluffyPbkdfKey_uptr encodedKey;
    if (ok)
    {
        encodedKey.reset(FluffyPbkdfKey_new());
        FluffyPbkdfInfo *info = encodedKey->content->info;
        info->iv = ASN1_OCTET_STRING_new();
        ossl::fromUInt32(*info->version, 0);
        ossl::fromUInt32(*info->iters, KDF_ITERS);
        ossl::fromBin(*info->salt, BinData(salt, sizeof(salt)));
        ossl::fromBin(*info->iv, BinData(iv, sizeof(iv)));
        ossl::fromNid(info->kdf, KDF_NID);
        ossl::fromNid(info->keytype, KEY_NID);
        ossl::fromBin(*encodedKey->content->payload, encipheredKey);
        content = ossl::encode(*encodedKey->content, i2d_FluffyPbkdfContent);
    }

    // Sign
    if (ok)
    {
        BinData signature = sign(content, cipher, mak);
        ok = !signature.empty();
        ossl::fromBin(*encodedKey->signature, signature);
    }

    // Encode
    BinData ret;
    if (ok)
        ret = ossl::encode(*encodedKey, i2d_FluffyPbkdfKey);

    return ret;
}

EvpPkeyPtr PbkdfKey::unwrap(const BinData &key, const BinData &pass)
{
    bool ok = true;

    // Sane validate input
    if (pass.empty())
        ok = log::error("Null password provided to key PBKDF.");

    // Decode encrypted key
    FluffyPbkdfKey_uptr encodedKey;
    if (ok)
    {
        encodedKey = ossl::decode<FluffyPbkdfKey_uptr>(key, d2i_FluffyPbkdfKey);
        if (!encodedKey)
            ok = log::error("Failed to decode PBKDF key.");
    }

    // Lookup MD/Cipher hashes
    const EVP_MD *md = nullptr;
    const EVP_CIPHER *cipher = nullptr;
    if (ok)
    {
        md = EVP_get_digestbynid(ossl::toNid(*encodedKey->content->info->kdf));
        cipher = EVP_get_cipherbynid(ossl::toNid(*encodedKey->content->info->keytype));
        if (!md)
            ok = log::error("Unknown PBKDF key KDF.");
        else if (!cipher)
            ok = log::error("Unknown PBKDF key cipher.");
    }

    // Validate decoded parameters
    BinData iv;
    BinData salt;
    uint64_t iters = 0;
    if (ok)
    {
        FluffyPbkdfInfo *info = encodedKey->content->info;

        uint64_t version = ossl::toUInt64(*info->version);
        iters = ossl::toUInt64(*info->iters);
        salt = ossl::toBin(*info->salt);
        if (info->iv)
            iv = ossl::toBin(*info->iv);
        size_t cipherIvLen = static_cast<size_t>(EVP_CIPHER_get_iv_length(cipher));

        if (version != 0)
            ok = log::error("Invalid PBKDF key version {}.", version);
        else if (iters < KDF_ITERS_MIN || iters > KDF_ITERS_MAX)
            ok = log::error("Invalid PBKDF iteration count {}.", iters);
        else if (salt.length() < SALT_LEN_MIN || salt.length() > SALT_LEN_MAX)
            ok = log::error("Invalid PBKDF salt length {}.", salt.length());
        else if (iv.length() != cipherIvLen)
            ok = log::error("Invalid PBKDF iv length {} (Expected {}).", iv.length(), cipherIvLen);
    }


    // Generate 2x AES-256 keys from password
    SafeData dek;
    SafeData mak;
    if (ok)
    {
        ok = pbkdf(
            static_cast<size_t>(EVP_CIPHER_get_key_length(cipher)),
            pass,
            salt,
            static_cast<int>(iters),
            md,
            dek,
            mak);
    }

    // Verify signature
    if (ok)
    {
        BinData content = ossl::encode(*encodedKey->content, i2d_FluffyPbkdfContent);
        BinData verify = sign(content, cipher, mak);
        BinData signature = ossl::toBin(*encodedKey->signature);
        if (signature.length() != verify.length() || memcmp(signature.data(), verify.data(), signature.length()) != 0)
            ok = log::error("Failed to verify PBKDF key signature.");
    }

    // Decrypt private key
    SafeData decrypted;
    if (ok)
    {
        BinData payload = ossl::toBin(*encodedKey->content->payload);

        int clearLen = 0;
        int finalLen = 0;
        decrypted.resize(payload.length());

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        EVP_DecryptInit_ex(ctx, cipher, nullptr, dek.data(), iv.data());
        EVP_DecryptUpdate(ctx, decrypted.data(), &clearLen, payload.data(), static_cast<int>(payload.length()));
        int ret = EVP_DecryptFinal_ex(ctx, decrypted.data() + clearLen, &finalLen);
        EVP_CIPHER_CTX_free(ctx);

        if (ret != 1 || clearLen < 0 || finalLen < 0)
            ok = log::error("Failed to decrypt PBKDF key payload: {}", ossl::Error::pop());
        else
            decrypted.resize(static_cast<size_t>(clearLen + finalLen));
    }

    // Decode private key
    EvpPkeyPtr ret;
    if (ok)
    {
        ret = Curve25519::fromPrivate(decrypted);
        if (!ret)
            ok = log::error("Failed to decode PBKDF key payload.");
    }

    return ret;
}
