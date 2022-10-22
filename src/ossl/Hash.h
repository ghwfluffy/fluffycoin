#pragma once

#include <fluffycoin/utils/BinData.h>

namespace fluffycoin
{

namespace ossl
{

namespace Hash
{
    BinData sha2_256(const BinData &data);
    BinData sha3_256(const BinData &data);
}

}

}
