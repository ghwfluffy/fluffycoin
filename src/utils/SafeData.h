#pragma once

#include <fluffycoin/utils/BinData.h>

namespace fluffycoin
{

/**
 * Binary data but the memory is overwritten before free
 */
class SafeData : public BinData
{
    public:
        SafeData();
        SafeData(const unsigned char *data, size_t len);
        SafeData(const std::string &data);
        SafeData(SafeData &&);
        SafeData(const SafeData &);
        SafeData &operator=(SafeData &&);
        SafeData &operator=(const SafeData &);
        SafeData(BinData &&);
        SafeData(const BinData &);
        SafeData &operator=(BinData &&);
        SafeData &operator=(const BinData &);
        ~SafeData() = default;

        using BinData::zeroize;
};

}
