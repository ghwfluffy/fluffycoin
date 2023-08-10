#pragma once

#include <fluffycoin/utils/BinData.h>

#include <string>
#include <stdint.h>

namespace fluffycoin::db
{

class IResultCol
{
    public:
        virtual ~IResultCol() = default;

        virtual BinData data() const = 0;
        virtual std::string str() const = 0;
        virtual uint32_t UInt32() const = 0;
        virtual uint64_t UInt64() const = 0;
        virtual int32_t Int32() const = 0;
        virtual int64_t Int64() const = 0;
        virtual bool boolean() const = 0;

    protected:
        IResultCol() = default;
        IResultCol(IResultCol &&) = default;
        IResultCol(const IResultCol &) = default;
        IResultCol &operator=(IResultCol &&) = default;
        IResultCol &operator=(const IResultCol &) = default;
};

}
