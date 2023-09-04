#pragma once

#include <fluffycoin/curator/DbIntegrator.h>

#include <fluffycoin/async/boost.h>

#include <map>
#include <list>
#include <atomic>
#include <memory>
#include <condition_variable>

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

        void start();
        void stop();

        void addBlock(
            BinData data,
            uint32_t recon,
            uint32_t shard,
            uint32_t block);

    private:
        DbIntegrator dbIntegrator;

        struct PendingShardBlock
        {
            uint32_t block = 0;
            block::Block data;
        };
        struct PendingRecons
        {
            block::Block data;
            std::map<uint32_t, std::list<PendingShardBlocks>> pendingBlocks;
        };
        std::map<uint32_t, BinData> pendingRecons;

        std::mutex mtx;
        std::condition_variable;
        std::atomic<bool> running;
        std::unique_ptr<std::thread> thread;
};

}
