#include <fluffycoin/curator/LoadFromFs.h>
#include <fluffycoin/curator/FsStorage.h>

#include <fluffycoin/db/Database.h>

#include <fluffycoin/async/BuriedTreasure.h>

#include <fluffycoin/log/Log.h>

#include <boost/asio/steady_timer.hpp>

using namespace fluffycoin;
using namespace fluffycoin::curator;

#if 0
namespace
{

/**
 * State/memory that will last through the sequence of
 * asynchronous operation to load the latest blocks
 * from the filesystem to the database
 */
struct LoadStateTreasure : async::Treasure
{
    const svc::ServiceScene &ctx;
    db::Session dbsession;
    uint64_t reloadFrom = UINT64_MAX;

    uint64_t dbCursorRecon = 0;
    std::map<uint32_t, uint32_t> shardToBlockCursor;

    std::unique_ptr<boost::asio::steady_timer> delay;

    bool complete = false;

    LoadStateTreasure(
        const svc::ServiceScene &ctx)
            : ctx(ctx)
    {}

    ~LoadStateTreasure()
    {
        if (complete)
            log::info("LoadFromFs complete.");
        else
            log::info("LoadFromFs shutting down.");
    }
};
using LoadState = async::BuriedTreasure<LoadStateTreasure>;

void delayedRetry(LoadState state);

void loadNextBlock(LoadState state)
{
    //FsStorage &fs = state().ctx.get<FsStorage>();
    // TODO
    (void)state;
}

void findDbCursor(LoadState state)
{
    // What is the highest reconciliation block known to the db
    Details details;
    // TODO: Move and use on same line
    state().dbsession.select("SELECT reconciliation FROM blocks ORDER BY reconciliation DESC LIMIT 1",
        details,
        [state = std::move(state)](db::DataResult result, Details &details) mutable -> void
        {
            if (!details.isOk())
            {
                details.log().error(log::Db, "Failed to retrieve max db reconciliation block.");
                delayedRetry(std::move(state));
                return;
            }

            // Pull out result
            if (!result.getNumResults())
                log::info("No block data in database.");
            else
                state().dbCursorRecon = result.next()[0].UInt64();

            // Start loading in from filesystem
            loadNextBlock(std::move(state));
        });
}

void startDbSession(LoadState state)
{
    // Start a new DB session
    Details details;
    db::Database &db = state().ctx.get<db::Database>();
    db.newReadOnlySession(
        details,
        [state = std::move(state)](db::Session session, Details &details) mutable -> void
        {
            // Failed to connect: Delayed retry
            if (!details.isOk())
            {
                details.log().error(log::Db, "Failed to connect to database.");
                delayedRetry(std::move(state));
                return;
            }

            state().dbsession = std::move(session);
            // Figure out where the DB is
            findDbCursor(std::move(state));
        });
}

void delayedRetry(LoadState state)
{
    state().delay = std::make_unique<boost::asio::steady_timer>(state().ctx.asio, std::chrono::seconds(10));
    state().delay->async_wait( // TODO: Move and use on same line
        [state = std::move(state)](const boost::system::error_code &ec) mutable -> void
        {
            // ASIO shutting down
            if (ec == boost::asio::error::operation_aborted)
                return;

            // Connect to DB
            startDbSession(std::move(state));
        });
}

}

#endif

async::Ret<void> LoadFromFs::init(
    const svc::ServiceScene &ctx,
    uint64_t reloadFrom)
{
#if 0
    // Setup async state
    LoadState state(ctx.trove, ctx);
    state().reloadFrom = reloadFrom;

    // Connect to DB
    startDbSession(std::move(state));
#endif
    (void)reloadFrom;
    Details details;
    db::Session session = co_await ctx.get<db::Database>().newReadOnlySession(details);
}
