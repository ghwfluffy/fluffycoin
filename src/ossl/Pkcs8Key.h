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
    BinData wrap(const EVP_PKEY &key, const BinData &pass);
    EvpPkeyPtr unwrap(const BinData &key, const BinData &pass);
}

}

}
