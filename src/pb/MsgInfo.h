#pragma once

#include <google/protobuf/message.h>

namespace fluffycoin::pb
{

namespace MsgInfo
{
    std::string getType(
        const google::protobuf::Message &msg);
}

}
