#include <fluffycoin/ossl/Initialize.h>

#include <atomic>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>

using namespace fluffycoin;

namespace
{

std::atomic<int> giInitRef{0};

}

void ossl::Initialize::initialize()
{
    if (giInitRef++ != 0)
        return;

    OPENSSL_init();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
}

void ossl::Initialize::cleanup()
{
    if (--giInitRef != 0)
        return;

    ERR_free_strings();
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    OPENSSL_cleanup();
}
