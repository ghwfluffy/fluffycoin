#include <fluffycoin/pb/Catalog.h>

#include <fluffycoin/log/Log.h>

using namespace fluffycoin;
using namespace fluffycoin::pb;

namespace
{

std::map<std::string, const google::protobuf::Message *> mapDefaults;
std::map<std::string, size_t> mapIds;

std::string getTypeFromUrl(
    const std::string &url)
{
    if (url.rfind("fc/", 0) == std::string::npos)
        return url;
    return url.substr(3);
}

}

void Catalog::registerMsg(
    size_t id,
    const google::protobuf::Message &msg)
{
    std::string type = MsgInfo::getType(msg);
    mapIds[type] = id;
    mapDefaults[type] = &msg;

    log::traffic("Registered protobuf message type '{}'.", type);
}

size_t Catalog::getTypeId(const std::string &type)
{
    std::string msgType = getTypeFromUrl(type);
    auto iter = mapIds.find(msgType);
    return iter == mapIds.end() ? 0 : iter->second;
}

size_t Catalog::getTypeId(const google::protobuf::Message &msg)
{
    return getTypeId(MsgInfo::getType(msg));
}

std::unique_ptr<google::protobuf::Message> Catalog::newMsg(const std::string &type)
{
    std::string msgType = getTypeFromUrl(type);
    auto iter = mapDefaults.find(msgType);
    if (iter != mapDefaults.end())
        return std::unique_ptr<google::protobuf::Message>(iter->second->New());
    return std::unique_ptr<google::protobuf::Message>();
}

void Catalog::cleanup()
{
    mapIds.clear();
    mapDefaults.clear();
}
