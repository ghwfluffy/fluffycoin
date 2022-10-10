#pragma once

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

#include <memory>

namespace fluffycoin
{

namespace ossl
{

/**
 * Decode binary data into an OpenSSL struct
 */
template<typename uptr, typename D2I>
uptr decode(const BinData &binData, D2I pfDecode)
{
    if (binData.empty())
        return uptr();

    const unsigned char *ptr = binData.data();
    return uptr(pfDecode(nullptr, &ptr, static_cast<long>(binData.length())));
}

}

}
