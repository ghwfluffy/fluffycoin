#pragma once

#include <stddef.h>

namespace fluffycoin::db
{

class Result
{
    public:
        Result();
        Result(Result &&) = default;
        Result(const Result &) = default;
        Result &operator=(Result &&) = default;
        Result &operator=(const Result &) = default;
        ~Result() = default;

        size_t getAffectedRows() const;
        void setAffectedRows(size_t ui);

    private:
        size_t uiAffectedRows;
};

}
