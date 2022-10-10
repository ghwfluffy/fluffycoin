#pragma once

#include <openssl/evp.h>
#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/ossl/EvpPkeyPtr.h>

namespace fluffycoin
{

namespace ossl
{

namespace PbkdfKeyEncode
{
    BinData encode(const EVP_PKEY &key, const BinData &pass);
    EvpPkeyPtr decode(const BinData &key, const BinData &pass);
}

}

}
