#include <fluffycoin/ossl/Entropy.h>
#include <fluffycoin/ossl/Error.h>
#include <fluffycoin/ossl/Prng.h>

#include <fluffycoin/log/Log.h>

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <sys/time.h>
#include <sys/sysinfo.h>

using namespace fluffycoin;

namespace
{

size_t osEntropy(
    unsigned char *buffer,
    size_t dataLen)
{
    size_t len = 0;
    FILE *fp = fopen("/dev/urandom", "rb");
    if (!fp)
        log::error("Failed to open urandom ({}): {}.", errno, strerror(errno));
    else
    {
        len = fread(buffer, 1, dataLen, fp);
        fclose(fp);
    }

    return len;
}

void osAesSeed()
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    constexpr const unsigned int ROUNDS = 10;
    for (unsigned int ui = 0; ui < ROUNDS; ui++)
    {
        // Get some entropy from the OS
        constexpr const size_t INPUT_LEN = 2048;
        unsigned char buffer[INPUT_LEN];
        size_t len = osEntropy(buffer, sizeof(buffer));

        // Shouldn't happen
        if (len != sizeof(buffer))
            log::error("Failed to read full buffer from urandom ({}/{}).", len, sizeof(buffer));

        // Need at least an AES-128 blocks for this
        constexpr const size_t AES_LEN = 16;
        if (len < AES_LEN)
        {
            log::error("Not enough entropy for seed algorithm.");
            continue;
        }

        // Get some entropy from OpenSSL's current DRBG context
        unsigned char key_iv[AES_LEN * 2];
        int ret = RAND_bytes(key_iv, static_cast<int>(sizeof(key_iv)));
        if (ret != 1)
        {
            log::error("Failed to generate AES key for random seed: {}.", ossl::Error::pop());
            continue;
        }

        // Use the key to encipher the data
        EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key_iv, key_iv + AES_LEN);
        unsigned char enciphered[INPUT_LEN];
        int enclen = 0;
        ret = EVP_EncryptUpdate(ctx, buffer, &enclen, enciphered, static_cast<int>(len));
        if (ret != 1 || enclen <= 0)
        {
            log::error("Failed to encipher random seed: {}.", ossl::Error::pop());
            continue;
        }

        // Seed with enciphered data
        ossl::Prng::seed(enciphered, static_cast<size_t>(enclen));
    }

    EVP_CIPHER_CTX_free(ctx);
}

void stackSeed()
{
    constexpr const size_t STACK_SEED_LEN = 32 * 1024;
    unsigned char data[STACK_SEED_LEN];
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    ossl::Prng::seed(data, sizeof(data));
    #pragma GCC diagnostic pop
}

void sysInfoSeed()
{
    struct sysinfo info = {};
    int ret = sysinfo(&info);
    if (ret != 0)
        log::error("Failed to get system info seed ({}): {}.", errno, strerror(errno));
    else
        ossl::Prng::seed(&info, sizeof(info));
}

void timeSeed()
{
    struct timeval tv = {};
    int ret = gettimeofday(&tv, nullptr);
    if (ret != 0)
        log::error("Failed to get system time seed ({}): {}.", errno, strerror(errno));
    else
        ossl::Prng::seed(&tv, sizeof(tv));
}

}

/**
 * Mixes in extra entropy to OpenSSL's DRBG.
 * It should have plenty of entropy already without this,
 * we're just being eccentric and paranoid.
 */
void ossl::Entropy::seed()
{
    stackSeed();
    sysInfoSeed();
    timeSeed();
    osAesSeed();
}
