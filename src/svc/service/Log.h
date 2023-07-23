#pragma once

#include <fluffycoin/log/Log.h>

namespace fluffycoin::svc
{

enum class Log
{
    Api,
    Event,
};

void initLogger();

}
