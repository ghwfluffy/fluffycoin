#pragma once

#include <openssl/evp.h>
#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/ossl/EvpPkeyPtr.h>

namespace fluffycoin
{

namespace ossl
{

/**
 * Protect the keys at rest using PBKDFv2 encryption
 * in an proprietary format
 */
namespace PbkdfKey
{
    BinData wrap(const EVP_PKEY &key, const BinData &pass);
    EvpPkeyPtr unwrap(const BinData &key, const BinData &pass);
}

}

}
