#pragma once

#include <fluffycoin/utils/BinData.h>

#include <iostream>

namespace fluffycoin
{
    std::ostream &operator<<(std::ostream &os, const BinData &data);
}
