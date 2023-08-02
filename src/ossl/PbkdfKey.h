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
    constexpr const uint32_t KDF_ITERS_DEFAULT = 1024 * 1024;

    BinData wrap(
        const EVP_PKEY &key,
        const BinData &pass,
        unsigned int KDF_ITERS = KDF_ITERS_DEFAULT);

    EvpPkeyPtr unwrap(
        const BinData &key,
        const BinData &pass);
}

}

}
