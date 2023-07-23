#include <fluffycoin/svc/ServiceScene.h>

using namespace fluffycoin;
using namespace fluffycoin::svc;

ServiceScene::ServiceScene(
    std::function<void(ServiceStatusCode)> statusCallback,
    boost::asio::io_context &asio,
    async::TreasureTrove &trove)
        : asio(asio)
        , trove(trove)
        , statusUpdate(statusCallback)
{
}

void ServiceScene::setStatus(ServiceStatusCode code)
{
    statusUpdate(code);
}

void *ServiceScene::getPtr(size_t t) const
{
    auto iter = utils.find(t);
    return iter == utils.end() ? nullptr : iter->second;
}

void ServiceScene::setPtr(size_t t, void *p)
{
    utils[t] = p;
}
