#include <fluffycoin/db/Database.h>
#include <fluffycoin/db/priv/SessionImpl.h>

#include <ozo/ozo.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/redirect_error.hpp>

using namespace fluffycoin;
using namespace fluffycoin::db;

namespace fluffycoin::db
{

struct DatabaseImpl
{
    std::unique_ptr<db::priv::Ozo::ConnectionPool> pool;
};

}

Database::Database(boost::asio::io_context &io)
        : io(io)
{
}

Database::Database(Database &&rhs)
        : io(rhs.io)
        , impl(std::move(rhs.impl))
{
}

Database &Database::operator=(Database &&rhs)
{
    if (this != &rhs)
        impl = std::move(rhs.impl);
    return (*this);
}

Database::~Database()
{
}

void Database::connect(
    const std::string &params,
    Details &details)
{
    // Static ozo configuration
    ozo::connection_pool_config connection_pool_config;
    connection_pool_config.capacity = 1;
    connection_pool_config.queue_capacity = 10;
    connection_pool_config.idle_timeout = std::chrono::seconds(60);
    // Maximum lifetime for a connection prevents issues with postgres cache fragmentation
    connection_pool_config.lifespan = std::chrono::hours(24);

    // Connect
    impl = std::make_unique<DatabaseImpl>();
    try {
        const ozo::connection_info connection_info(params);
        impl->pool = std::make_unique<db::priv::Ozo::ConnectionPool>(connection_info, connection_pool_config);
        log::info(log::Db, "Database pool initialized.");
    } catch (const std::exception &e) {
        details.setError(log::Db, ErrorCode::DatabaseError, "connect",
            "Failed to connect to database: {}", e.what());
    }
}

async::Ret<Session> Database::newSession(
    Details &details)
{
    if (!impl)
    {
        details.setError(log::Db, ErrorCode::NotConnected, "new_session",
            "Database is not connected.");
        co_return Session();
    }

    db::priv::Ozo::ConnectionProvider conn = (*impl->pool)[io];

    boost::system::error_code ec;
    db::priv::Ozo::Transaction trans = co_await ozo::begin(
        conn,
        boost::asio::redirect_error(boost::asio::use_awaitable, ec));

    if (ec)
    {
        details.setError(log::Db, ErrorCode::ConnectError, "new_session",
            "Failed to start database transaction: {}.",
            db::priv::Ozo::error(ec, trans));
        co_return Session();
    }

    std::unique_ptr<db::priv::SessionImpl> sess = std::make_unique<db::priv::SessionImpl>();
    sess->transaction = std::move(trans);
    sess->mode = db::priv::SessionImpl::Mode::Transaction;
    details.log().traffic(log::Db, "Created new transaction session.");

    co_return Session(std::move(sess));
}

async::Ret<Session> Database::newReadOnlySession(
    Details &details)
{
    if (!impl)
    {
        details.setError(log::Db, ErrorCode::NotConnected, "new_session",
            "Database is not connected.");
        co_return Session();
    }

    db::priv::Ozo::ConnectionProvider provider = (*impl->pool)[io];

    boost::system::error_code ec;
    db::priv::Ozo::Connection conn = co_await ozo::get_connection(
        std::move(provider),
        std::chrono::seconds(20),
        boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec)
    {
        details.setError(log::Db, ErrorCode::ConnectError, "new_session",
            "Failed to connect to database: {}",
            db::priv::Ozo::error(ec, conn));
        co_return Session();
    }

    std::unique_ptr<db::priv::SessionImpl> sess = std::make_unique<db::priv::SessionImpl>();
    sess->connection = std::move(conn);
    sess->mode = db::priv::SessionImpl::Mode::Connection;
    details.log().traffic(log::Db, "Created new read only session.");

    co_return Session(std::move(sess));
}
