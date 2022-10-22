#include "fluffytest.h"

#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/utils/Hex.h>

#include <fluffytest/utils/BinData.h>

#include <iostream>

namespace fluffycoin
{
    std::ostream &operator<<(std::ostream &os, const BinData &data)
    {
        return os << Hex::encode(data);
    }
}
