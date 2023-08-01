#include "fluffytest.h"

#include <fluffycoin/alg/Base58.h>

using namespace fluffycoin;
using namespace fluffycoin::alg;

namespace
{

void encodeTest(
    const unsigned char *value,
    size_t valueLen,
    const char *encodedValue)
{
    BinData bin(value, valueLen);

    std::string encoded = Base58::encode(bin);
    EXPECT_EQ(encoded, encodedValue);

    BinData decoded = Base58::decode(encoded);
    EXPECT_EQ(decoded, bin);
}

void encodeTest(
    const char *value,
    const char *encodedValue)
{
    return encodeTest(reinterpret_cast<const unsigned char *>(value), strlen(value), encodedValue);
}

}

TEST(Base58, Test1)
{
    const unsigned char value[] = {0x07, 0x5B, 0xCD, 0x15};
    encodeTest(value, sizeof(value), "BukQL");
}

TEST(Base58, Test2)
{
    const unsigned char value[] = {0x27, 0x0f};
    encodeTest(value, sizeof(value), "3yQ");
}

TEST(Base58, Test3)
{
    encodeTest("Hello World!", "2NEpo7TZRRrLZSi2U");
}

TEST(Base58, Test4)
{
    encodeTest(
        "The quick brown fox jumps over the lazy dog.",
        "USm3fpXnKG5EUBx2ndxBDMPVciP5hGey2Jh4NDv6gmeo1LkMeiKrLJUUBk6Z");
}

TEST(Base58, Test5)
{
    const unsigned char value[] = {0x00, 0x00, 0x28, 0x7f, 0xb4, 0xcd};
    encodeTest(value, sizeof(value), "11233QC4");
}
