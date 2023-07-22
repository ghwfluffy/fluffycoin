#include <fluffycoin/pb/MsgInfo.h>

using namespace fluffycoin;
using namespace fluffycoin::pb;

std::string MsgInfo::getType(
    const google::protobuf::Message &msg)
{
    return msg.GetDescriptor()->full_name();
}
