#pragma once

#include <fluffycoin/db/IResultRow.h>

namespace fluffycoin
{

namespace db
{

class IResultData
{
    public:
        virtual ~IResultData() = default;

        virtual size_t getNumResults() const = 0;

        virtual const IResultRow &cur() const = 0;
        virtual const IResultRow &next() const = 0;

    protected:
        IResultData() = default;
        IResultData(IResultData &&) = default;
        IResultData(const IResultData &) = default;
        IResultData &operator=(IResultData &&) = default;
        IResultData &operator=(const IResultData &) = default;
};


}

}
