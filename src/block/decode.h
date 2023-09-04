#pragma once

#include <fluffycoin/block/Block.h>
#include <fluffycoin/block/Genesis.h>
#include <fluffycoin/block/Reconciliation.h>

#include <fluffycoin/utils/BinData.h>

namespace fluffycoin
{

namespace block
{

bool decode(const BinData &data, Hash &hash);
bool decode(const BinData &data, Block &block);
bool decode(const BinData &data, Genesis &genesis);
bool decode(const BinData &data, Reconciliation &recon);

}

}
