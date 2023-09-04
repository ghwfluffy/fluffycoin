#include <fluffycoin/curator/LoadFromFs.h>
#include <fluffycoin/curator/fs/BlockStorage.h>

#include <fluffycoin/db/Database.h>

#include <fluffycoin/async/BuriedTreasure.h>

#include <fluffycoin/log/Log.h>

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

    std::unique_ptr<asn1::Block> asn1Block = block::
    asn1::Block asn1Block;
    

    asn1::block::Block block(
        // In/Out
        BinData readBlock(
            uint32_t reconciliation,
            uint32_t shard,
            uint32_t block) const;
    std::file = alg::BlockFilename::get(recon, shard, block);
    if (!FileTools::read(file, data))
        details.setError("Failed to read {}.", 

    // TODO
}
#endif

#if 0
async::Ret<void> decodeGenesis(
    Details &details)
{
    block::Genesis block;
    if (!block::decode(data, block))
    {
        details.setError("Failed to decode genesis block.");
        fs.clearBlock(recon, shard, block);
        co_return;
    }

    // TODO
}

async::Ret<void> loadBlock(
    fs::BlockStorage &fs,
    uint32_t recon,
    uint32_t shard,
    uint32_t block,
    Details &details)
{
    BinData data = fs.readBlock(recon, shard, block);
    if (data.empty())
    {
        details.setError("Failed to read block ({}.{}.{}).", recon, shard, block);
        fs.clearBlock(recon, shard, block);
        co_return;
    }

    // Genesis block
    if (recon == 0 && shard == 0 && block == 0)
    {
        co_await decodeGenesis(details);
        co_return;
    }
    // Reconciliation block
    else if (shard == 0 && block == 0)
    {
        co_await decodeRecon(recon, details);
        co_return;
    }

    // Normal block
    co_await decodeBlock(recon, shard, block, details);
    co_return;
}
#endif

async::Ret<void> LoadFromFs::init(
    const svc::ServiceScene &ctx,
    uint64_t reloadFrom)
{
    (void)reloadFrom; // TODO
    Details details;

    // Need to have at least the genesis and reconciliation block to start decoding
    fs::BlockStorage &fs = ctx.get<fs::BlockStorage>();
    uint32_t endRecon = fs.getEndReconciliation();
    if (endRecon == 0)
    {
        details.setError(log::Init, ErrorCode::InternalError, "load_from_fs", "No genesis block available.");
        co_return;
    }

    if (endRecon == 1)
    {
        details.setError(log::Init, ErrorCode::InternalError, "load_from_fs", "No initial reconciliation block available.");
        co_return;
    }

#if 0
    db::Session session = co_await ctx.get<db::Database>().newSession(details);

    // TODO
    log::info("Loading {} reconciliation blocks from filesystem.", endRecon > 0 ? (endRecon - 1) : 0);
    for (uint32_t uiRecon = 0; uiRecon < endRecon; uiRecon++)
    {
        uint32_t numShards = fs.getNumShards(uiRecond);
        log::info("Loading {} shards for reconciliation block {}.", numShards, uiRecon);
        for (uint32_t uiShard = 0; uiShard < numShards; uiShard++)
        {
            uint32_t numBlocks = fs.getNumBlocks(uiRecond, uiShard);
            log::info("Loading {} blocks for shard {} of reconciliation block {}.", numBlocks, uiShard, uiRecon);
            for (uint32_t uiBlock = 0; uiBlock < numBlocks; uiBlock++)
            {
                loadBlock(fs, uiRecon, uiShard, uiBlock, details);
            }
        }
    }
#endif
}
