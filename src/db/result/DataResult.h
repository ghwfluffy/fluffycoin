#pragma once

#include <fluffycoin/db/IResultData.h>

#include <memory>

namespace fluffycoin::db
{

class DataResult
{
    public:
        DataResult() = default;
        DataResult(std::unique_ptr<IResultData> data);
        DataResult(DataResult &&) = default;
        DataResult(const DataResult &) = delete;
        DataResult &operator=(DataResult &&) = default;
        DataResult &operator=(const DataResult &) = delete;
        ~DataResult() = default;

        size_t getNumResults() const;

        const IResultRow &cur() const;
        const IResultRow &next() const;

    private:
        std::unique_ptr<IResultData> data;
};

}
