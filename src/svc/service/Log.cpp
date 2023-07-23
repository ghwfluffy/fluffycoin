#include <fluffycoin/svc/Log.h>

#include <fluffycoin/pb/Logger.h>
#include <fluffycoin/log/Category.h>

using namespace fluffycoin;
using namespace fluffycoin::svc;

void svc::initLogger()
{
    log::Category::add(Log::Api, "API");
    log::Category::add(Log::Event, "EVENT");
    pb::Logger::init();
}
