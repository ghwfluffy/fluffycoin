#include "fluffytest.h"

#include <fluffycoin/db/Database.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>

using namespace fluffycoin;

namespace
{

struct TestContext
{
    boost::asio::io_context io;
    db::Database db;

    TestContext()
        : db(io)
    {}
};

}

// Tag any test that uses the psql docker with `Postgres_`
TEST(Postgres_Database, Connect)
{
    TestContext ctx;

    // Initialize ozo
    Details details;
    ctx.db.connect(fluffytest::db::Params::get(), details);
    EXPECT_TRUE(details.isOk());

    // Coroutine to start transaction
    auto AsyncFunc = [&]() -> async::Ret<void>
    {
        db::Session sess = co_await ctx.db.newSession(details);
        EXPECT_TRUE(details.isOk());
        co_return;
    };

    // Run coroutine
    boost::asio::co_spawn(ctx.io, AsyncFunc(), boost::asio::detached);
    ctx.io.run();
}

TEST(Postgres_Database, ConnectReadOnly)
{
    TestContext ctx;

    // Initialize ozo
    Details details;
    ctx.db.connect(fluffytest::db::Params::get(), details);
    EXPECT_TRUE(details.isOk());

    // Coroutine to start transaction
    auto AsyncFunc = [&]() -> async::Ret<void>
    {
        db::Session sess = co_await ctx.db.newReadOnlySession(details);
        EXPECT_TRUE(details.isOk());
        co_return;
    };

    // Run coroutine
    boost::asio::co_spawn(ctx.io, AsyncFunc(), boost::asio::detached);
    ctx.io.run();
}
