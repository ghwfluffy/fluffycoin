#pragma once

#include <fluffycoin/ossl/EvpPkeyPtr.h>

#include <fluffycoin/utils/SafeData.h>

namespace fluffycoin
{

namespace ossl
{

namespace Curve25519
{
    constexpr const size_t PUBLIC_LEN = 32;

    EvpPkeyPtr generate();

    BinData toPublic(const EVP_PKEY &key);
    EvpPkeyPtr fromPublic(const BinData &publicPoint);

    SafeData toPrivate(const EVP_PKEY &key);
    EvpPkeyPtr fromPrivate(const BinData &priv);

    BinData sign(const EVP_PKEY &key, const BinData &data);
    bool verify(const EVP_PKEY &key, const BinData &data, const BinData &signature);
}

}

}
