#include <fluffycoin/curator/fs/BlockStorage.h>

#include <fluffycoin/alg/BlockFilename.h>

#include <fluffycoin/utils/FileTools.h>

using namespace fluffycoin;
using namespace fluffycoin::curator;
using namespace fluffycoin::curator::fs;

void BlockStorage::setDirectory(
    const std::string &dir)
{
    this->dir = dir;
}

BinData BlockStorage::readBlock(
    uint32_t reconciliation,
    uint32_t shard,
    uint32_t block) const
{
    std::string file = dir + "/" + alg::BlockFilename::get(reconciliation, shard, block);

    BinData data;
    if (!FileTools::read(file, data))
        data.clear();
    return data;
}

bool BlockStorage::writeBlock(
    uint32_t reconciliation,
    uint32_t shard,
    uint32_t block,
    const BinData &data) const
{
    std::string file = dir + "/" + alg::BlockFilename::get(reconciliation, shard, block);
    return FileTools::write(file, data);
}

uint32_t BlockStorage::getEndReconciliation() const
{
    // TODO
    return 0;
}

uint32_t BlockStorage::getNumShards(
    uint32_t reconciliation) const
{
    (void)reconciliation;
    // TODO
    return 0;
}

uint32_t BlockStorage::getNumBlocks(
    uint32_t reconciliation,
    uint32_t shard) const
{
    (void)reconciliation;
    (void)shard;
    // TODO
    return 0;
}
