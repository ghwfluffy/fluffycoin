#pragma once

#include <fluffycoin/utils/BinData.h>

#include <memory>

namespace fluffycoin
{

namespace ossl
{

/**
 * Encode OpenSSL structs into binary data
 */
template<typename DataType=BinData, typename T, typename I2D>
DataType encode(const T &tStruct, I2D pfEncode)
{
    DataType ret;
    if (!pfEncode)
        return ret;

    int len = pfEncode(&tStruct, nullptr);
    if (len > 0)
    {
        ret.resize(static_cast<size_t>(len));
        unsigned char *ptr = ret.data();
        pfEncode(&tStruct, &ptr);
    }

    return ret;
}

}

}
