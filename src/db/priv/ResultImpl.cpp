#include <fluffycoin/db/priv/ResultImpl.h>
#include <fluffycoin/db/priv/pg_type_d.h>

#include <fluffycoin/utils/Strings.h>

#include <fluffycoin/log/Log.h>

#include <libpq-fe.h>
#include <ozo/pg/handle.h>

#include <arpa/inet.h>

using namespace fluffycoin;
using namespace fluffycoin::db;
using namespace fluffycoin::db::priv;

namespace
{

constexpr uint64_t ntohll(uint64_t value)
{
    constexpr const uint32_t test = 1;
    return
        (reinterpret_cast<const unsigned char *>(&test)[0] == 1) ?
            // Convert little endian to big endian
            (
                ((value & 0xFF00000000000000ULL) >> 56) |
                ((value & 0x00FF000000000000ULL) >> 40) |
                ((value & 0x0000FF0000000000ULL) >> 24) |
                ((value & 0x000000FF00000000ULL) >> 8) |
                ((value & 0x00000000FF000000ULL) << 8) |
                ((value & 0x0000000000FF0000ULL) << 24) |
                ((value & 0x000000000000FF00ULL) << 40) |
                ((value & 0x00000000000000FFULL) << 56)
            ) :
            // Big endian
            value;
}

}

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

BinData ResultCol::data() const
{
    if (row >= numRows || col >= numCols)
        return BinData();

    const char *psz = PQgetvalue(result, row, col);
    int length = PQgetlength(result, row, col);
    if (!psz || length <= 0 || PQgetisnull(result, row, col))
        return BinData();
    return BinData(reinterpret_cast<const unsigned char *>(psz), static_cast<size_t>(length));
}

std::string ResultCol::str() const
{
    if (row >= numRows || col >= numCols)
        return std::string();

    const char *psz = PQgetvalue(result, row, col);
    int length = PQgetlength(result, row, col);
    if (!psz || length <= 0 || PQgetisnull(result, row, col))
        return std::string();

    std::string val;
    Oid type = PQftype(result, col);
    switch (type)
    {
        case INT2OID:
        case INT4OID:
        case INT8OID:
        {
            val = fluffycoin::to_string(Int64());
            break;
        }
        case BOOLOID:
        {
            val = Int64() ? "1" : "0";
            break;
        }
        case CHAROID:
        case TEXTOID:
        case VARCHAROID:
        {
            val.assign(psz, static_cast<size_t>(length));
            break;
        }
        default:
            log::error(log::Db, "Support needs to be added for postgres string type {}.", type);
            break;
    }

    return val;
}

uint32_t ResultCol::UInt32() const
{
    if (row >= numRows || col >= numCols)
        return 0;

    int64_t val = Int64();
    return val < 0 ? 0 : (val > UINT32_MAX ? UINT32_MAX : static_cast<uint32_t>(val));
}

uint64_t ResultCol::UInt64() const
{
    if (row >= numRows || col >= numCols)
        return 0;

    int64_t val = Int64();
    return val < 0 ? 0 : static_cast<uint64_t>(val);
}

int32_t ResultCol::Int32() const
{
    if (row >= numRows || col >= numCols)
        return 0;
    int64_t val = Int64();
    return val < INT32_MIN ? INT32_MIN : (val > INT32_MAX ? INT32_MAX : static_cast<int32_t>(val));
}

int64_t ResultCol::Int64() const
{
    if (row >= numRows || col >= numCols)
        return 0;

    int64_t val = 0;
    const char *psz = PQgetvalue(result, row, col);
    int length = PQgetlength(result, row, col);
    if (!psz || length <= 0 || PQgetisnull(result, row, col))
        return val;

    Oid type = PQftype(result, col);
    switch (type)
    {
        case INT8OID:
        case CHAROID:
        {
            memcpy(&val, psz, sizeof(val));
            val = static_cast<int64_t>(ntohll(static_cast<uint64_t>(val)));
            break;
        }
        case INT2OID:
        {
            uint8_t val8 = 0;
            memcpy(&val8, psz, sizeof(val8));
            val = static_cast<int64_t>(ntohs(val8));
            break;
        }
        case INT4OID:
        {
            uint32_t val32 = 0;
            memcpy(&val32, psz, sizeof(val32));
            val = static_cast<int64_t>(ntohl(val32));
            break;
        }
        case BOOLOID:
        {
            uint8_t val8 = 0;
            memcpy(&val8, psz, sizeof(val8));
            val = bool(val8);
            break;
        }
        case VARCHAROID:
        case TEXTOID:
        {
            val = fluffycoin::from_string<int64_t>(str());
            break;
        }
        default:
            log::error(log::Db, "Support needs to be added for postgres integer type {}.", type);
            break;
    }

    return val;
}

bool ResultCol::boolean() const
{
    if (row >= numRows || col >= numCols)
        return false;

    const char *psz = PQgetvalue(result, row, col);
    int length = PQgetlength(result, row, col);
    if (!psz || length <= 0 || PQgetisnull(result, row, col))
        return false;

    Oid type = PQftype(result, col);
    switch (type)
    {
        case INT8OID:
        case CHAROID:
        case INT2OID:
        case INT4OID:
            return bool(UInt64());
        case BOOLOID:
        {
            uint8_t val8 = 0;
            memcpy(&val8, psz, sizeof(val8));
            return bool(val8);
        }
        case VARCHAROID:
        case TEXTOID:
            return fluffycoin::from_string<bool>(str());
        default:
            log::error(log::Db, "Support needs to be added for postgres boolean type {}.", type);
            break;
    }

    return false;
}

bool ResultCol::null() const
{
    if (row >= numRows || col >= numCols)
        return true;
    return PQgetisnull(result, row, col);
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

ResultImpl::ResultImpl(ozo::result resultIn)
        : result(std::move(resultIn))
        , row(result.native_handle())
{
}

size_t ResultImpl::getNumResults() const
{
    PGresult *presult = result.native_handle();
    int rows = presult ? PQntuples(presult) : 0;
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
