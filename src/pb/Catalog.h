#pragma once

#include <fluffycoin/pb/MsgInfo.h>

#include <google/protobuf/message.h>

namespace fluffycoin::pb
{

/**
 * Keeps track of the known protobuf types and
 * can build them from type strings
 */
namespace Catalog
{
    template<typename T>
    void registerMsg()
    {
        registerMsg(typeid(T).hash_code(), T::default_instance());
    }

    void registerMsg(size_t id, const google::protobuf::Message &msg);

    template<typename T>
    std::string getType()
    {
        return MsgInfo::getType(T::default_instance());
    }

    size_t getTypeId(const std::string &type);
    size_t getTypeId(const google::protobuf::Message &msg);
    std::unique_ptr<google::protobuf::Message> newMsg(const std::string &type);

    void cleanup();
}

}
