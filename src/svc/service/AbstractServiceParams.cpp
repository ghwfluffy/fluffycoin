#include <fluffycoin/svc/AbstractServiceParams.h>

using namespace fluffycoin;
using namespace fluffycoin::svc;

void AbstractServiceParams::initCmdLineParams(ArgParser &args) const
{
    (void)args;
}

void AbstractServiceParams::setCmdLineArgs(const Args &args)
{
    (void)args;
}

std::string AbstractServiceParams::getLogFile() const
{
    return std::string();
}

unsigned int AbstractServiceParams::getNumEventThreads() const
{
    return 1;
}

uint16_t AbstractServiceParams::getApiPort() const
{
    return 0;
}

uint16_t AbstractServiceParams::getEventPort() const
{
    return 0;
}

const BinData &AbstractServiceParams::getServerKey() const
{
    static BinData b;
    return b;
}

void AbstractServiceParams::setupScene(ServiceScene &ctx)
{
    (void)ctx;
}

void AbstractServiceParams::addApiHandlers(ApiHandlerMap &handlers, bool paused) const
{
    (void)handlers;
    (void)paused;
}

void AbstractServiceParams::addEventSubscriptions(EventSubscriptionMap &handlers) const
{
    (void)handlers;
}

IAuthenticator *AbstractServiceParams::getAuthenticator()
{
    return nullptr;
}

bool AbstractServiceParams::preInit()
{
    return true;
}

bool AbstractServiceParams::init()
{
    return true;
}

void AbstractServiceParams::cleanup()
{
}

void AbstractServiceParams::postCleanup()
{
}
