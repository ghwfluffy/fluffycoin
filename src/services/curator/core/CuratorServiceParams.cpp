#include <fluffycoin/curator/CuratorServiceParams.h>
#include <fluffycoin/curator/LoadFromFs.h>

#include <fluffycoin/utils/FileTools.h>

using namespace fluffycoin;
using namespace fluffycoin::curator;

CuratorServiceParams::CuratorServiceParams()
{
    pctx = nullptr;
    reloadFrom = UINT64_MAX;
    fsStorageDir = "~/.fluffycoin/blocks";
}

std::string CuratorServiceParams::getLogFile() const
{
    return "/var/log/fluffycoin/curator.log";
}

void CuratorServiceParams::setupScene(svc::ServiceScene &ctx)
{
    pctx = &ctx;
    // TODO: OZO DB implementation
    //db = std::make_unique<db::Database>(ctx.asio);
    ctx.set(*db.get());
    ctx.set(fs);
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
        reloadFrom = static_cast<uint64_t>(args.getSizeT("reload-from"));
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

#if 0
bool CuratorServiceParams::preInit(bool paused)
{
    // TODO
    return false;
}
#endif

bool CuratorServiceParams::init(bool paused)
{
    // Don't do anything in paused state
    if (paused)
        return true;

    fs.setDirectory(fsStorageDir);

    LoadFromFs::init(*pctx, reloadFrom);
    return true;
}

#if 0
void CuratorServiceParams::cleanup()
{
    // TODO
}

void CuratorServiceParams::postCleanup()
{
    // TODO
}
#endif
