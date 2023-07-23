#pragma once

#include <fluffycoin/async/AlwaysCallback.h>

#include <google/protobuf/message.h>

namespace fluffycoin::svc
{

// This gets triggered at the end of API command processing to respond to client
using ApiResponseCallback = async::AlwaysCallback<void(std::unique_ptr<google::protobuf::Message>)>;

}
