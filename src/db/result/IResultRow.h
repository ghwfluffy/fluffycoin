#pragma once

#include <fluffycoin/db/IResultCol.h>

namespace fluffycoin::db
{

class IResultRow
{
    public:
        ~IResultRow() = default;

        virtual operator bool() const = 0;

        virtual const IResultCol &operator[](size_t pos) const = 0;

    protected:
        IResultRow() = default;
        IResultRow(IResultRow &&) = default;
        IResultRow(const IResultRow &) = default;
        IResultRow &operator=(IResultRow &&) = default;
        IResultRow &operator=(const IResultRow &) = default;
};

}
