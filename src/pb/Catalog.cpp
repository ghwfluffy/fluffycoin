#include <fluffycoin/pb/Catalog.h>

using namespace fluffycoin;
using namespace fluffycoin::pb;

namespace
{

std::map<std::string, const google::protobuf::Message *> mapDefaults;
std::map<std::string, size_t> mapIds;

}

void Catalog::registerMsg(
    size_t id,
    const google::protobuf::Message &msg)
{
    std::string type = MsgInfo::getType(msg);
    mapIds[type] = id;
    mapDefaults[type] = &msg;
}

size_t Catalog::getTypeId(const std::string &type)
{
    auto iter = mapIds.find(type);
    return iter == mapIds.end() ? 0 : iter->second;
}

std::unique_ptr<google::protobuf::Message> Catalog::newMsg(const std::string &type)
{
    auto iter = mapDefaults.find(type);
    if (iter != mapDefaults.end())
        return std::unique_ptr<google::protobuf::Message>(iter->second->New());
    return std::unique_ptr<google::protobuf::Message>();
}

void Catalog::cleanup()
{
    mapIds.clear();
    mapDefaults.clear();
}
