#pragma once

#include <fluffycoin/utils/BinData.h>

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
    if (binData.empty() || !pfDecode)
        return uptr();

    const unsigned char *ptr = binData.data();
    return uptr(pfDecode(nullptr, &ptr, binData.length());
}

}

}
