#pragma once

#include <fluffycoin/log/Log.h>

namespace fluffycoin::svc
{

enum class Log
{
    Service,
    Api,
    Event,
};

void initLogger();

}
