#include <fluffycoin/db/Session.h>
#include <fluffycoin/db/priv/SessionImpl.h>
#include <fluffycoin/db/priv/ResultImpl.h>

#include <ozo/execute.h>
#include <ozo/request.h>
#include <ozo/query.h>
#include <ozo/shortcuts.h>

#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/redirect_error.hpp>

using namespace fluffycoin;
using namespace fluffycoin::db;

namespace
{

size_t getAffectedRows(
    ozo::result &result)
{
    PGresult *presult = result.native_handle();
    const char *sz = PQcmdTuples(presult);
    int affectedRows = sz ? atoi(sz) : 0;
    return affectedRows > 0 ? static_cast<size_t>(affectedRows) : 0;
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
    if (impl && impl->transaction)
        log::error(log::Db, "Database transaction never finalized.");
}

async::Ret<void> Session::commit(
    Details &details)
{
    // Commit
    if (!impl || !impl->transaction)
    {
        details.setError(log::Db, ErrorCode::InternalError, "commit",
            "No database session active.");
    }
    else
    {
        boost::system::error_code ec;
        auto conn = co_await ozo::commit(std::move(impl->transaction),
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        if (ec)
        {
            details.setError(log::Db, ErrorCode::DatabaseError, "query",
                "Failed to finalize database session: {}.",
                db::priv::Ozo::error(ec, conn));
        }
        else
        {
            details.log().traffic(log::Db, "Finalized database transaction.");
        }
    }

    impl.reset();

    co_return;
}

async::Ret<Result> Session::query(
    const std::string &query,
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
        ozo::result result;
        auto trans = co_await ozo::request(std::move(impl->transaction), ozo::make_query(query), ozo::into(result),
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        // Check error
        if (ec)
        {
            details.setError(log::Db, ErrorCode::DatabaseError, "query",
                "Failed to query database: {}.",
                db::priv::Ozo::error(ec, trans));
        }
        // Get affected rows
        else
        {
            ret.setAffectedRows(getAffectedRows(result));
        }
        impl->transaction = std::move(trans);
    }
    else
    {
        ozo::result result;
        auto conn = co_await ozo::request(std::move(impl->connection), ozo::make_query(query), ozo::into(result),
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        // Check error
        if (ec)
        {
            details.setError(log::Db, ErrorCode::DatabaseError, "query",
                "Failed to query database: {}.",
                db::priv::Ozo::error(ec, conn));
        }
        // Get affected rows
        else
        {
            ret.setAffectedRows(getAffectedRows(result));
        }
        impl->connection = std::move(conn);
    }

    // Log affected rows count
    if (details.isOk())
        details.log().traffic(log::Db, "{} affected rows.", ret.getAffectedRows());

    co_return std::move(ret);
}

async::Ret<DataResult> Session::select(
    const std::string &query,
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
    ozo::result result;
    boost::system::error_code ec;
    if (impl->isTransaction())
    {
        auto trans = co_await ozo::request(std::move(impl->transaction), ozo::make_query(query), ozo::into(result),
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        // Check error
        if (ec)
        {
            details.setError(log::Db, ErrorCode::DatabaseError, "query",
                "Failed to query database: {}.",
                db::priv::Ozo::error(ec, trans));
        }
        impl->transaction = std::move(trans);
    }
    else
    {
        auto conn = co_await ozo::request(std::move(impl->connection), ozo::make_query(query), ozo::into(result),
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        // Check error
        if (ec)
        {
            details.setError(log::Db, ErrorCode::DatabaseError, "query",
                "Failed to query database: {}.",
                db::priv::Ozo::error(ec, conn));
        }
        impl->connection = std::move(conn);
    }

    // Convert results into fluffycoin format
    DataResult ret;
    if (details.isOk())
        ret = DataResult(std::make_unique<db::priv::ResultImpl>(std::move(result)));

    co_return std::move(ret);
}
