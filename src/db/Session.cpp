#include <fluffycoin/db/Session.h>
#include <fluffycoin/db/priv/SessionImpl.h>
#include <fluffycoin/db/priv/ResultImpl.h>

#include <ozo/execute.h>
#include <ozo/request.h>
#include <ozo/query.h>

#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/redirect_error.hpp>

using namespace fluffycoin;
using namespace fluffycoin::db;

namespace
{

template<typename Conn>
size_t getAffectedRows(Conn &conn)
{
    auto uptrResult = ozo::pg::make_safe(PQgetResult(get_native_handle(conn)));
    const char *sz = PQcmdTuples(uptrResult.get());
    int affectedRows = sz ? atoi(sz) : 0;
    if (affectedRows > 0)
        return static_cast<size_t>(affectedRows);
    return 0;
}

}

Session::Session()
{
}

Session::Session(std::unique_ptr<priv::SessionImpl> impl)
        : impl(std::move(impl))
{
}

Session::Session(Session &&rhs)
        : impl(std::move(rhs.impl))
{
}

Session &Session::operator=(Session &&rhs)
{
    if (this != &rhs)
        impl = std::move(rhs.impl);

    return (*this);
}

Session::~Session()
{
}

async::Ret<Result> Session::query(
    std::string query,
    Details &details)
{
    if (!impl)
    {
        details.setError(log::Db, ErrorCode::NotConnected, "db_session",
            "Database session not initialized.");
        co_return Result();
    }

    // Log query
    details.log().traffic(log::Db, "{}", query);

    // Do query
    Result ret;
    boost::system::error_code ec;
    if (impl->isTransaction())
    {
        auto trans = co_await ozo::execute(std::move(impl->transaction), ozo::make_query(query),
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        // Check error
        if (ec)
        {
            details.setError(log::Db, ErrorCode::DatabaseError, "query",
                "Failed to query database: {}.",
                db::priv::Ozo::error(ec, trans));
        }
        else
        {
            ret.setAffectedRows(getAffectedRows(trans));
        }
        impl->transaction = std::move(trans);
    }
    else
    {
        auto conn = co_await ozo::execute(std::move(impl->connection), ozo::make_query(query),
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        // Check error
        if (ec)
        {
            details.setError(log::Db, ErrorCode::DatabaseError, "query",
                "Failed to query database: {}.",
                db::priv::Ozo::error(ec, conn));
        }
        else
        {
            ret.setAffectedRows(getAffectedRows(conn));
        }
        impl->connection = std::move(conn);
    }

    // Log affected rows count
    if (details.isOk())
        details.log().traffic(log::Db, "{} affected rows.", ret.getAffectedRows());

    co_return std::move(ret);
}

async::Ret<DataResult> Session::select(
    std::string query,
    Details &details)
{
    if (!impl)
    {
        details.setError(log::Db, ErrorCode::NotConnected, "db_session",
            "Database session not initialized.");
        co_return DataResult();
    }

    // Log query
    details.log().traffic(log::Db, "{}", query);

    // Do query
    std::unique_ptr<pg_result, ozo::pg::safe_handle<pg_result>::deleter> result;
    boost::system::error_code ec;
    if (impl->isTransaction())
    {
        auto trans = co_await ozo::execute(std::move(impl->transaction), ozo::make_query(query),
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        // Check error
        if (ec)
        {
            details.setError(log::Db, ErrorCode::DatabaseError, "query",
                "Failed to query database: {}.",
                db::priv::Ozo::error(ec, trans));
        }
        else
        {
            result = ozo::pg::make_safe(PQgetResult(get_native_handle(trans)));
        }
        impl->transaction = std::move(trans);
    }
    else
    {
        auto conn = co_await ozo::execute(std::move(impl->connection), ozo::make_query(query),
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        // Check error
        if (ec)
        {
            details.setError(log::Db, ErrorCode::DatabaseError, "query",
                "Failed to query database: {}.",
                db::priv::Ozo::error(ec, conn));
        }
        else
        {
            result = ozo::pg::make_safe(PQgetResult(get_native_handle(conn)));
        }
        impl->connection = std::move(conn);
    }

    // Convert results into fluffycoin format
    DataResult ret;
    if (details.isOk())
        ret = DataResult(std::make_unique<db::priv::ResultImpl>(std::move(result)));

    co_return std::move(ret);
}
