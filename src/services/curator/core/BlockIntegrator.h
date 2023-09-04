#pragma once

#include <fluffycoin/curator/DbIntegrator.h>

#include <fluffycoin/db/Database.h>

#include <fluffycoin/async/Ret.h>

#include <map>
#include <mutex>

namespace fluffycoin::curator
{

/**
 * Integrates blocks into the current blockchain state
 */
class BlockIntegrator
{
    public:
        BlockIntegrator(
            db::Database &database);
        BlockIntegrator(BlockIntegrator &&) = delete;
        BlockIntegrator(const BlockIntegrator &) = delete;
        BlockIntegrator &operator=(BlockIntegrator &&) = delete;
        BlockIntegrator &operator=(const BlockIntegrator &) = delete;
        ~BlockIntegrator() = default;

        async::Ret<void> addBlock(
            BinData data,
            uint32_t recon,
            uint32_t shard,
            uint32_t block);

    private:
        async::Ret<bool> tryNext();
        async::Ret<void> tryIntegrate();
        void removeBlock(
            uint32_t recon,
            uint32_t shard,
            uint32_t block);

        DbIntegrator dbIntegrator;

        struct PendingBlock
        {
            bool init = false;
            uint32_t recon = 0;
            uint32_t shard = 0;
            uint32_t block = 0;
            block::Block data;
        };

        struct PendingShard
        {
            std::map<uint32_t, block::Block> pendingBlocks;
        };
        struct PendingRecon
        {
            std::map<uint32_t, PendingShard> pendingShards;
        };
        std::map<uint32_t, PendingRecon> pendingRecons;

        std::mutex mtx;

        uint32_t shardCursor;
};

}
