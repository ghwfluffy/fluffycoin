#pragma once

#include <fluffycoin/utils/BinData.h>

namespace fluffycoin
{

namespace ossl
{

/**
 * Encode OpenSSL structs into binary data
 */
template<typename uptr, typename I2D>
BinData encode(uptr ptStruct, I2D pfEncode)
{
    BinData ret;
    if (!ptStruct || !pfEncode)
        return ret;

    int len = pfEncode(ptStruct.get(), nullptr);
    if (len > 0)
    {
        ret.resize(static_cast<size_t>(len));
        unsigned char *ptr = ret.data();
        pfEncode(ptStruct, &ptr);
    }

    return ret;
}

}

}
