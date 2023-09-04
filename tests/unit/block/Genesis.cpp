#include "fluffytest.h"

#include <fluffycoin/block/Genesis.h>

#include <fluffycoin/alg/info.h>
#include <fluffycoin/alg/Address.h>

#include <fluffycoin/ossl/Curve25519.h>

using namespace fluffycoin;

TEST(Genesis, Encode)
{
    // Generate keypair
    ossl::EvpPkeyPtr key = ossl::Curve25519::generate();
    ASSERT_NE(key.get(), nullptr);

    BinData address = alg::Address::generate(*key, "GHW");

    // Setup Genesis block
    block::Genesis genesis;
    genesis.setProtocol(alg::PROTOCOL_VERSION);
    genesis.setName(alg::BLOCKCHAIN_NAME);
    genesis.setVersion(alg::BLOCKCHAIN_VERSION);
    genesis.setCreation(block::Time::now());
    genesis.setCreator(address);

    block::Specie greed;
    greed.setCoins(10 * 1000);
    genesis.setGreed(greed);
    genesis.setSeed(BinData("GHW"));

    BinData serial = genesis.encode();
    EXPECT_FALSE(serial.empty());
}
