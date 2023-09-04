#include <fluffycoin/curator/LoadFromFs.h>
#include <fluffycoin/curator/BlockIntegrator.h>
#include <fluffycoin/curator/fs/BlockStorage.h>

#include <fluffycoin/alg/BlockFilename.h>

#include <fluffycoin/utils/FileTools.h>

#include <fluffycoin/log/Log.h>

using namespace fluffycoin;
using namespace fluffycoin::curator;

namespace
{

async::Ret<void> loadFile(
    const svc::ServiceScene &ctx,
    uint32_t reloadFrom,
    const std::string &file)
{
    // Get metadata from filename
    uint32_t curRecon = 0;
    uint32_t curShard = 0;
    uint32_t curBlock = 0;
    if (!alg::BlockFilename::parse(file, curRecon, curShard, curBlock))
    {
        log::error(log::Init, "Invalid file '{}' in block directory.", file);
        co_return;
    }

    // We don't need to load this one
    if (curRecon < reloadFrom)
        co_return;

    // Read data
    fs::BlockStorage &fs = ctx.get<fs::BlockStorage>();
    BinData blockData = fs.readBlock(curRecon, curShard, curBlock);
    if (blockData.empty())
    {
        log::error(log::Init, "Failed to read file '{}'.", file);
        co_return;
    }

    // Hand it off to the integrator
    BlockIntegrator &integrator = ctx.get<BlockIntegrator>();
    co_await integrator.addBlock(std::move(blockData), curRecon, curShard, curBlock);
    co_return;
}

}

async::Ret<void> LoadFromFs::init(
    const svc::ServiceScene &ctx,
    uint32_t reloadFrom)
{
    reloadFrom = 0; // TODO

    // Get all available blocks
    std::list<std::string> files;
    fs::BlockStorage &fs = ctx.get<fs::BlockStorage>();
    if (!fs.getFiles(files))
    {
        log::error(log::Init, "Failed to read blocks from filesystem.");
        co_return;
    }

    // For each file
    for (const std::string &file : files)
        co_await loadFile(ctx, reloadFrom, file);

    co_return;
}
