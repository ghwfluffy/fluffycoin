#include <fluffycoin/svc/RequestScene.h>

using namespace fluffycoin;
using namespace fluffycoin::svc;

RequestScene::RequestScene(
    const ServiceScene &svcScene)
        : svcScene(&svcScene)
{
}

void RequestScene::setRequest(std::unique_ptr<google::protobuf::Message> msg)
{
    this->msg = std::move(msg);
}

Details &RequestScene::details()
{
    return deets;
}

log::Logger &RequestScene::log()
{
    return deets.log();
}

const svc::ServiceScene &RequestScene::utils() const
{
    return *svcScene;
}

bool RequestScene::isOk() const
{
    return deets.isOk();
}

const ErrorStatus &RequestScene::getError() const
{
    return deets.error();
}

void RequestScene::operator+=(Details &details)
{
    this->deets += std::move(details);
}
