#pragma once

#include <fluffycoin/block/Genesis.h>

#include <fluffycoin/utils/BinData.h>

namespace fluffycoin
{

namespace block
{

BinData encode(const Genesis &);

}

}
