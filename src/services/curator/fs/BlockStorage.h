#pragma once

#include <fluffycoin/utils/BinData.h>

#include <string>
#include <stdint.h>

namespace fluffycoin::curator::fs
{

/**
 * Interface for reading and writing blocks to the filesystem
 */
class BlockStorage
{
    public:
        BlockStorage() = default;
        BlockStorage(BlockStorage &&) = default;
        BlockStorage(const BlockStorage &) = default;
        BlockStorage &operator=(BlockStorage &&) = default;
        BlockStorage &operator=(const BlockStorage &) = default;
        ~BlockStorage() = default;

        // Init
        void setDirectory(
            const std::string &dir);

        // In/Out
        BinData readBlock(
            uint32_t reconciliation,
            uint32_t shard,
            uint32_t block) const;

        bool writeBlock(
            uint32_t reconciliation,
            uint32_t shard,
            uint32_t block,
            const BinData &data) const;

        // Info
        uint32_t getEndReconciliation() const;

        uint32_t getNumShards(
            uint32_t reconciliation) const;

        uint32_t getNumBlocks(
            uint32_t reconciliation,
            uint32_t shard) const;

    private:
        std::string dir;
};

}
