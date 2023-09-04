#include <fluffycoin/curator/BlockIntegrator.h>

#include <fluffycoin/block/decode.h>

using namespace fluffycoin;
using namespace fluffycoin::curator;

BlockIntegrator::BlockIntegrator(
    db::Database &database)
        : dbIntegrator(database)
{
    shardCursor = 0;
}

async::Ret<void> BlockIntegrator::addBlock(
    BinData binData,
    uint32_t recon,
    uint32_t shard,
    uint32_t block)
{
    block::Block data;
    if (!block::decode(binData, data))
    {
        log::error("Invalid block data."); // XXX: Bubble up
        co_return;
    }

    // Append the pending list
    {
        // Find the recon
        std::lock_guard<std::mutex> lock(mtx);
        auto iterRecon = pendingRecons.find(recon);
        if (iterRecon == pendingRecons.end())
            iterRecon = pendingRecons.emplace(recon, PendingRecon()).first;

        // Find the shard
        PendingRecon &pendingRecon = iterRecon->second;
        auto iterShard = pendingRecon.pendingShards.find(shard);
        if (iterShard == pendingRecon.pendingShards.end())
            iterShard = pendingRecon.pendingShards.emplace(shard, PendingShard()).first;

        // Place the block into the shard's map
        PendingShard &pendingShard = iterShard->second;
        pendingShard.pendingBlocks.emplace(block, std::move(data));

        if (shardCursor > shard)
            shardCursor = shard;
    }

    // Attempt to integrate pending blocks
    co_await tryIntegrate();
    co_return;
}

async::Ret<void> BlockIntegrator::tryIntegrate()
{
    bool retry = true;
    while (retry)
        retry = co_await tryNext();
}

namespace
{

bool isSuccess(
    DbIntegrator::Result ret)
{
    return ret == DbIntegrator::Result::Success;
}

bool isBadBlock(
    DbIntegrator::Result ret)
{
    return
        ret == DbIntegrator::Result::InvalidIndex ||
        ret == DbIntegrator::Result::Duplicate ||
        ret == DbIntegrator::Result::BadBlock ||
        ret == DbIntegrator::Result::NoMatch ||
        ret == DbIntegrator::Result::AlreadyDone;
}

}

async::Ret<bool> BlockIntegrator::tryNext()
{
    PendingBlock nextBlock;

    // Get next pending block
    {
        std::lock_guard<std::mutex> lock(mtx);

        // Get the next thing to integrate
        auto iterRecon = pendingRecons.begin();
        if (iterRecon == pendingRecons.end())
        {
            // Nothing to do
            co_return false;
        }

        uint32_t recon = iterRecon->first;
        PendingRecon &pendingRecon = iterRecon->second;

        // Empty pending recon list
        if (pendingRecon.pendingShards.empty())
        {
            pendingRecons.erase(iterRecon);
            co_return true;
        }

        // Try to integrate each block that is pending
        std::map<uint32_t, PendingShard> &pendingShards = pendingRecon.pendingShards;
        for (const auto &pair : pendingShards)
        {
            uint32_t shard = pair.first;
            if (shard < shardCursor)
                continue;

            const std::map<uint32_t, block::Block> &pendingBlocks = pair.second.pendingBlocks;
            for (const auto &pairBlock : pendingBlocks)
            {
                uint32_t block = pairBlock.first;
                const block::Block &data = pairBlock.second;

                nextBlock.init = true;
                nextBlock.data = data;
                nextBlock.recon = recon;
                nextBlock.shard = shard;
                nextBlock.block = block;
                break;
            }

            if (nextBlock.init)
                break;
        }
    }

    bool tryNext = false;
    if (nextBlock.init)
    {
        Details details;
        DbIntegrator::Result ret = co_await dbIntegrator.integrate(
            nextBlock.data,
            nextBlock.recon,
            nextBlock.shard,
            nextBlock.block,
            details);

        tryNext = isSuccess(ret);
        // Done with this block
        if (isSuccess(ret) || isBadBlock(ret))
            removeBlock(nextBlock.recon, nextBlock.shard, nextBlock.block);
    }

    co_return tryNext;
}

void BlockIntegrator::removeBlock(
    uint32_t recon,
    uint32_t shard,
    uint32_t block)
{
    std::lock_guard<std::mutex> lock(mtx);

    auto iterRecon = pendingRecons.find(recon);
    if (iterRecon == pendingRecons.end())
        return;

    std::map<uint32_t, PendingShard> &pendingShards = iterRecon->second.pendingShards;
    auto iterShard = pendingShards.find(shard);
    if (iterShard == pendingShards.end())
        return;

    std::map<uint32_t, block::Block> &pendingBlocks = iterShard->second.pendingBlocks;
    auto iterBlock = pendingBlocks.find(block);
    if (iterBlock != pendingBlocks.end())
        pendingBlocks.erase(iterBlock);

    // Cleanup empty lists
    if (pendingBlocks.empty())
        pendingShards.erase(iterShard);
    if (pendingShards.empty())
        pendingRecons.erase(iterRecon);
}
