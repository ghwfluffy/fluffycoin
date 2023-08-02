#pragma once

#include <openssl/evp.h>
#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/ossl/EvpPkeyPtr.h>

namespace fluffycoin
{

namespace ossl
{

/**
 * Protect a private key at rest using PKCS #8 password based encryption
 */
namespace Pkcs8Key
{
    constexpr const unsigned int KDF_ITERS_DEFAULT = 1024 * 1024;

    BinData wrap(
        const EVP_PKEY &key,
        const BinData &pass,
        unsigned int iters = KDF_ITERS_DEFAULT);

    EvpPkeyPtr unwrap(
        const BinData &key,
        const BinData &pass);
}

}

}
