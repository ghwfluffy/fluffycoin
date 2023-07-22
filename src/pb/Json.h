#pragma once

#include <fluffycoin/utils/Details.h>

#include <google/protobuf/message.h>

namespace fluffycoin::pb
{

/**
 * Serialize protobuf to/from JSON
 */
namespace Json
{
    void fromJson(
        const char *data,
        size_t len,
        google::protobuf::Message &msg,
        Details &details);

    void toJson(
        const google::protobuf::Message &msg,
        std::string &data,
        Details &details);
}

}
