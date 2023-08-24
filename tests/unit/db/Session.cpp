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

TEST(Postgres_Session, SimpleSelect)
{
    TestContext ctx;

    // Initialize ozo
    Details details;
    ctx.db.connect(fluffytest::db::Params::get(), details);
    EXPECT_TRUE(details.isOk());

    // Coroutine to run test
    auto AsyncFunc = [&]() -> async::Ret<void>
    {
        db::Session sess = co_await ctx.db.newReadOnlySession(details);
        EXPECT_TRUE(details.isOk());

        db::DataResult result = co_await sess.select("SELECT 1", details);
        EXPECT_TRUE(details.isOk());
        EXPECT_EQ(result.getNumResults(), 1);
        EXPECT_TRUE(result.next());
        EXPECT_EQ(result.cur()[0].UInt32(), 1);

        result = co_await sess.select("SELECT 123::BIGINT", details);
        EXPECT_TRUE(details.isOk());
        EXPECT_EQ(result.getNumResults(), 1);
        EXPECT_TRUE(result.next());
        EXPECT_EQ(result.cur()[0].UInt64(), 123);

        co_return;
    };

    // Run coroutine
    boost::asio::co_spawn(ctx.io, AsyncFunc(), boost::asio::detached);
    ctx.io.run();
}

TEST(Postgres_Session, SimpleInsert)
{
    TestContext ctx;

    // Initialize ozo
    Details details;
    ctx.db.connect(fluffytest::db::Params::get(), details);
    EXPECT_TRUE(details.isOk());

    // Coroutine to run test
    auto AsyncFunc = [&]() -> async::Ret<void>
    {
        db::Session sess = co_await ctx.db.newSession(details);
        EXPECT_TRUE(details.isOk());

        db::Result result = co_await sess.query("CREATE TABLE test (id INT NOT NULL, PRIMARY KEY (id))", details);
        EXPECT_TRUE(details.isOk());

        result = co_await sess.query("INSERT INTO test VALUES (1), (2), (3)", details);
        EXPECT_TRUE(details.isOk());
        EXPECT_EQ(result.getAffectedRows(), 3);

        result = co_await sess.query("DROP TABLE test", details);
        EXPECT_TRUE(details.isOk());
        co_return;
    };

    // Run coroutine
    boost::asio::co_spawn(ctx.io, AsyncFunc(), boost::asio::detached);
    ctx.io.run();
}
