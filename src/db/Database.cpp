#include <fluffycoin/db/Database.h>

#include <ozo/ozo.h>

using namespace fluffycoin;
using namespace fluffycoin::db;

namespace fluffycoin::db
{

struct DatabaseImpl
{
    using PoolType = ozo::connection_pool<
        ozo::connection_info<
            ozo::oid_map_t<>, ozo::none_t>,
        ozo::thread_safety<true> >;

    std::unique_ptr<PoolType> pool;
};

}

Database::Database()
{
}

Database::Database(Database &&rhs)
        : impl(std::move(rhs.impl))
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
        impl->pool = std::make_unique<DatabaseImpl::PoolType>(connection_info, connection_pool_config);
    } catch (const std::exception &e) {
        details.setError(log::Db, ErrorCode::DatabaseError, "connect",
            "Failed to connect to database: {}", e.what());
    }
}

async::Ret<Session> Database::newSession(Details &details)
{
    (void)details;
    co_return Session();
}

async::Ret<Session> Database::newReadOnlySession(Details &details)
{
    (void)details;
    co_return Session();
}
