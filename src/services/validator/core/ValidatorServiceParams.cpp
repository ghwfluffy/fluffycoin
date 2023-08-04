#include <fluffycoin/services/validator/core/ValidatorServiceParams.h>

using namespace fluffycoin;
using namespace fluffycoin::validator;

ValidatorServiceParams::ValidatorServiceParams()
{
    // TODO
}

std::string ValidatorServiceParams::getLogFile() const
{
    return "/var/log/fluffycoin/validator.log";
}

#if 0
void ValidatorServiceParams::initCmdLineParams(ArgParser &args) const
{
    // TODO
}

void ValidatorServiceParams::setCmdLineArgs(const Args &args)
{
    // TODO
}

unsigned int ValidatorServiceParams::getNumEventThreads() const
{
    // TODO
    return 1;
}

uint16_t ValidatorServiceParams::getApiPort() const
{
    // TODO
    return 0;
}

uint16_t ValidatorServiceParams::getEventPort() const
{
    // TODO
    return 0;
}

const BinData &ValidatorServiceParams::getServerKey() const
{
    // TODO
    return BinData();
}

void ValidatorServiceParams::setupScene(ServiceScene &ctx)
{
    // TODO
}

void ValidatorServiceParams::addApiHandlers(ApiHandlerMap &handlers, bool paused) const
{
    // TODO
}

void ValidatorServiceParams::addEventSubscriptions(EventSubscriptionMap &handlers) const
{
    // TODO
}

IAuthenticator *ValidatorServiceParams::getAuthenticator()
{
    // TODO
    return nullptr;
}

bool ValidatorServiceParams::preInit()
{
    // TODO
    return false;
}

bool ValidatorServiceParams::init()
{
    // TODO
    return false;
}

void ValidatorServiceParams::cleanup()
{
    // TODO
}

void ValidatorServiceParams::postCleanup()
{
    // TODO
}
#endif
