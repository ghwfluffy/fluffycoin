#include "fluffytest.h"

#include <fluffycoin/utils/Base64.h>

#include <fluffycoin/log/Log.h>

using namespace fluffycoin;

TEST(Base64, Encode)
{
    EXPECT_EQ(Base64::encode("Ghw"), std::string("R2h3"));
    EXPECT_EQ(Base64::encode("Gh"), std::string("R2g"));
    EXPECT_EQ(Base64::encode("G"), std::string("Rw"));
    EXPECT_EQ(Base64::encode(""), std::string(""));
    EXPECT_EQ(Base64::encode("RandomTest"), std::string("UmFuZG9tVGVzdA"));
}

TEST(Base64, Decode)
{
    EXPECT_EQ(Base64::decode("R2h3"), BinData("Ghw"));
    EXPECT_EQ(Base64::decode("R2g"), BinData("Gh"));
    EXPECT_EQ(Base64::decode("Rw"), BinData("G"));
    EXPECT_EQ(Base64::decode(""), BinData(""));
    EXPECT_EQ(Base64::decode("UmFuZG9tVGVzdA"), BinData("RandomTest"));
}
