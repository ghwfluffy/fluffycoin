#include <fluffycoin/utils/Hex.h>

using namespace fluffycoin;

namespace
{

char toChar(unsigned char uc)
{
    if (uc < 10)
        return static_cast<char>('0' + uc);
    return static_cast<char>('A' + uc - 10);
}

}

std::string Hex::encode(const BinData &binary)
{
    std::string ret;
    ret.resize(binary.length() * 2);

    const unsigned char *data = binary.data();
    for (size_t ui = 0; ui < binary.length(); ui++)
    {
        ret[ui * 2] = toChar(data[ui] >> 4);
        ret[ui * 2 + 1] = toChar(data[ui] & 0x0F);
    }

    return ret;
}
