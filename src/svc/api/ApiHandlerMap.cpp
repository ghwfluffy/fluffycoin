#include <fluffycoin/svc/ApiHandlerMap.h>

using namespace fluffycoin;
using namespace fluffycoin::svc;

const ApiHandlerMap::Handler *ApiHandlerMap::get(size_t reqType) const
{
    auto iter = handlers.find(reqType);
    return iter == handlers.end() ? nullptr : &iter->second;
}

void ApiHandlerMap::add(size_t reqType, Handler handler)
{
    handlers[reqType] = std::move(handler);
}
