#include <fluffycoin/ossl/Initialize.h>
#include <fluffycoin/ossl/Entropy.h>

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
    Entropy::seed();
}

void ossl::Initialize::cleanup()
{
    if (--giInitRef != 0)
        return;

    OPENSSL_cleanup();
}
