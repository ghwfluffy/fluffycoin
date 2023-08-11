#pragma once

#include <fluffycoin/utils/BinData.h>

#include <string>

namespace fluffycoin::curator
{

/**
 * Interface for reading and writing blocks to the filesystem
 */
class FsStorage
{
    public:
        FsStorage() = default;
        FsStorage(FsStorage &&) = default;
        FsStorage(const FsStorage &) = default;
        FsStorage &operator=(FsStorage &&) = default;
        FsStorage &operator=(const FsStorage &) = default;
        ~FsStorage() = default;

        void setDirectory(
            const std::string &dir) {this->dir = dir;} // TODO

        BinData readBlock(
            uint64_t reconciliation,
            uint32_t shard,
            uint32_t block) const;

        bool writeBlock(
            uint64_t reconciliation,
            uint32_t shard,
            uint32_t block,
            const BinData &data) const;

        BinData readSnapshot(
            uint64_t reconciliation);

        bool writeSnapshot(
            uint64_t reconciliation,
            const BinData &data);

    private:
        std::string dir;
};

}
