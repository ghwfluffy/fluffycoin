#include <fluffycoin/curator/CuratorServiceParams.h>
#include <fluffycoin/curator/LoadFromFs.h>

#include <fluffycoin/alg/DefaultPaths.h>

#include <fluffycoin/utils/FileTools.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>

using namespace fluffycoin;
using namespace fluffycoin::curator;

CuratorServiceParams::CuratorServiceParams()
{
    pctx = nullptr;
    reloadFrom = UINT32_MAX;
    fsStorageDir = alg::DefaultPaths::FS_BLOCKS;

    // Use FLUFFYCOIN_RESOURCE_DIR if set
    const char *resourceDir = getenv("FLUFFYCOIN_RESOURCE_DIR");
    if (resourceDir && *resourceDir)
    {
        fsStorageDir = resourceDir;
        fsStorageDir += "/blocks";
    }
}

std::string CuratorServiceParams::getLogFile() const
{
    return "/var/log/fluffycoin/curator.log";
}

void CuratorServiceParams::setupScene(svc::ServiceScene &ctx)
{
    pctx = &ctx;
    ctx.set(fsBlocks);
}

void CuratorServiceParams::initCmdLineParams(ArgParser &args) const
{
    args.addParam('b', "block-dir", "Location to read/write blocks from (Default: {})", fsStorageDir);
    args.addParam('r', "reload-from", "Reconciliation block to reload from (0=Reload all)");
}

void CuratorServiceParams::setCmdLineArgs(const Args &args)
{
    if (args.hasArg("block-dir"))
        fsStorageDir = args.getArg("block-dir");

    if (args.hasArg("reload-from"))
        reloadFrom = static_cast<uint32_t>(args.getSizeT("reload-from"));
}

uint16_t CuratorServiceParams::getApiPort() const
{
    return API_PORT;
}

uint16_t CuratorServiceParams::getEventPort() const
{
    return EVENT_PORT;
}

#if 0
void CuratorServiceParams::addApiHandlers(svc::ApiHandlerMap &handlers, bool paused) const
{
    if (paused)
        return;
}

void CuratorServiceParams::addEventSubscriptions(svc::EventSubscriptionMap &handlers) const
{
    // TODO
}
#endif

bool CuratorServiceParams::preInit(bool paused)
{
    // Don't do anything if we're paused
    if (paused)
        return true;

    // Already initialized
    if (db)
        return true;

    // Connect to database
    Details details(log::Init);
    db = std::make_unique<db::Database>(pctx->asio);
    db->connect(fmt::format("dbname={} host={} port={} user={} password={}",
        getenv("PGDATABASE"),
        getenv("PGHOST"),
        getenv("PGPORT"),
        getenv("PGUSER"),
        getenv("PGPASSWORD")), details);
    if (!details.isOk())
    {
        db.reset();
        return false;
    }

    // Create integrator thread
    integrator = std::make_unique<BlockIntegrator>(*db);

    // Add to service context
    pctx->set(*db.get());
    pctx->set(*integrator);

    // Connected
    return true;
}

bool CuratorServiceParams::init(bool paused)
{
    // Don't do anything in paused state
    if (paused)
        return true;

    // Load/Save blocks on filesystem
    fsBlocks.setDirectory(fsStorageDir);

    // Spawn coroutine to load filesystem blocks
    boost::asio::co_spawn(pctx->asio, LoadFromFs::init(*pctx, reloadFrom), boost::asio::detached);
    return true;
}

void CuratorServiceParams::postCleanup()
{
    // Done with integrator
    integrator.reset();
    // Done with database
    db.reset();
}
