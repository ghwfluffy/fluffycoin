#pragma once

#include <fluffycoin/utils/BinData.h>

#include <list>
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

        void clearBlock(
            uint32_t reconciliation,
            uint32_t shard,
            uint32_t block) const;

        bool getFiles(
            std::list<std::string> &files);

    private:
        std::string dir;
};

}
