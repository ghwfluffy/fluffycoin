#pragma once

#include <fluffycoin/db/IResultData.h>

#include <libpq-fe.h>
#include <ozo/pg/handle.h>

namespace fluffycoin::db::priv
{

class ResultCol : public IResultCol
{
    public:
        ResultCol(pg_result *result = nullptr);
        ResultCol(ResultCol &&) = default;
        ResultCol(const ResultCol &) = default;
        ResultCol &operator=(ResultCol &&) = default;
        ResultCol &operator=(const ResultCol &) = default;
        ~ResultCol() final = default;

        void setCol(int row, int col);

        BinData data() const final;
        std::string str() const final;
        uint32_t UInt32() const final;
        uint64_t UInt64() const final;
        int32_t Int32() const final;
        int64_t Int64() const final;
        bool boolean() const final;

    private:
        int col;
        int row;
        int numCols;
        int numRows;
        pg_result *result;
};

class ResultRow : public IResultRow
{
    public:
        ResultRow(pg_result *result = nullptr);
        ResultRow(ResultRow &&) = default;
        ResultRow(const ResultRow &) = default;
        ResultRow &operator=(ResultRow &&) = default;
        ResultRow &operator=(const ResultRow &) = default;
        ~ResultRow() final = default;

        operator bool() const final;

        void inc();

        const IResultCol &operator[](size_t pos) const final;

    private:
        pg_result *result;
        ResultCol col;
        int numRows;
        int curRow;
};

class ResultImpl : public IResultData
{
    public:
        ResultImpl(std::unique_ptr<pg_result, ozo::pg::safe_handle<pg_result>::deleter> result);
        ResultImpl(ResultImpl &&) = default;
        ResultImpl(const ResultImpl &) = delete;
        ResultImpl &operator=(ResultImpl &&) = default;
        ResultImpl &operator=(const ResultImpl &) = delete;
        ~ResultImpl() final = default;

        size_t getNumResults() const final;

        const IResultRow &cur() const final;
        const IResultRow &next() final;

    private:
        std::unique_ptr<pg_result, ozo::pg::safe_handle<pg_result>::deleter> result;
        ResultRow row;
};

}
