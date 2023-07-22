#pragma once

#include <fluffycoin/utils/Details.h>

#include <google/protobuf/any.pb.h>
#include <google/protobuf/message.h>

#include <memory>

namespace fluffycoin::pb
{

/**
 * Convert messages to/from 'Any' field type
 */
namespace AnyMessage
{
    void fromAny(
        const google::protobuf::Any &input,
        std::unique_ptr<google::protobuf::Message> &output,
        Details &details);

    void toAny(
        const google::protobuf::Message &input,
        google::protobuf::Any &output);
}

}
