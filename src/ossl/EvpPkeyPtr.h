#pragma once

#include <fluffycoin/ossl/uptr.h>

#include <openssl/evp.h>

namespace fluffycoin
{

namespace ossl
{
    FLUFFYCOIN_DEFINE_UPTR(EVP_PKEY)
    typedef EVP_PKEY_uptr EvpPkeyPtr;
}

}
