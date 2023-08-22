#include <fluffycoin/db/priv/ResultImpl.h>

#include <fluffycoin/utils/Strings.h>

#include <fluffycoin/log/Log.h>

#include <libpq-fe.h>
#include <ozo/pg/handle.h>

using namespace fluffycoin;
using namespace fluffycoin::db;
using namespace fluffycoin::db::priv;

ResultCol::ResultCol(pg_result *result)
    : col(0)
    , row(0)
    , result(result)
{
    numCols = result ? PQnfields(result) : 0;
    numRows = result ? PQntuples(result) : 0;
}

void ResultCol::setCol(int row, int col)
{
    this->row = row;
    this->col = col;
}

// TODO: I think ozo does something weird and these aren't going to be the format you usually expect
BinData ResultCol::data() const
{
    if (row >= numRows || col >= numCols)
        return BinData();

    const char *psz = PQgetvalue(result, row, col);
    int length = PQgetlength(result, row, col);
    if (!psz || length <= 0)
        return BinData();
    return BinData(reinterpret_cast<const unsigned char *>(psz), static_cast<size_t>(length));
}

std::string ResultCol::str() const
{
    if (row >= numRows || col >= numCols)
        return std::string();

    const char *psz = PQgetvalue(result, row, col);
    int length = PQgetlength(result, row, col);
    if (!psz || length <= 0)
        return std::string();
    return std::string(psz, static_cast<size_t>(length));
}

uint32_t ResultCol::UInt32() const
{
    if (row >= numRows || col >= numCols)
        return 0;

    return fluffycoin::from_string<uint32_t>(str());
}

uint64_t ResultCol::UInt64() const
{
    if (row >= numRows || col >= numCols)
        return 0;
    return fluffycoin::from_string<uint64_t>(str());
}

int32_t ResultCol::Int32() const
{
    if (row >= numRows || col >= numCols)
        return 0;
    return fluffycoin::from_string<int32_t>(str());
}

int64_t ResultCol::Int64() const
{
    if (row >= numRows || col >= numCols)
        return 0;
    return fluffycoin::from_string<int64_t>(str());
}

bool ResultCol::boolean() const
{
    if (row >= numRows || col >= numCols)
        return false;
    std::string strVal = str();
    return (strVal.length() == 1 && strVal[0] == 't') || fluffycoin::from_string<int>(strVal) != 0;
}

ResultRow::ResultRow(pg_result *result)
        : result(result)
        , col(result)
{
    curRow = 0;
    numRows = result ? PQntuples(result) : 0;
}

ResultRow::operator bool() const
{
    return result && numRows > 0 && curRow < numRows;
}

void ResultRow::inc()
{
    ++curRow;
}

const IResultCol &ResultRow::operator[](size_t pos) const
{
    ResultRow &THIS = const_cast<ResultRow &>(*this);
    if (THIS.curRow < 0)
    {
        THIS.curRow = 0;
        log::error(log::Db, "Incorrect usage of db::DataResult. Call next before retrieving data.");
    }
    THIS.col.setCol(curRow, static_cast<int>(pos));
    return col;
}

ResultImpl::ResultImpl(std::unique_ptr<pg_result, ozo::pg::safe_handle<pg_result>::deleter> result)
        : result(std::move(result))
        , row(this->result.get())
{
}

size_t ResultImpl::getNumResults() const
{
    int rows = result ? PQntuples(result.get()) : 0;
    return rows > 0 ? static_cast<size_t>(rows) : 0;
}

const IResultRow &ResultImpl::cur() const
{
    return row;
}

const IResultRow &ResultImpl::next()
{
    row.inc();
    return row;
}
