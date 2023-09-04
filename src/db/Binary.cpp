#include <fluffycoin/db/Binary.h>

using namespace fluffycoin;
using namespace fluffycoin::db;

namespace
{

bool isBinary(unsigned char uc)
{
    return uc < 0x20 || uc > 0x7E || uc == '\'' || uc == '\\';
}

}

std::string Binary::encode(
    const BinData &data)
{
    return encode(data.data(), data.length());
}

std::string Binary::encode(
    const unsigned char *data,
    size_t len)
{
    size_t numBinaryChars = 0;
    for (size_t ui = 0; ui < len; ui++)
        numBinaryChars += isBinary(data[ui]);

    size_t encodedLen = 9 + len + (numBinaryChars * 4);

    std::string ret;
    ret.reserve(encodedLen);
    ret += "'";

    for (size_t ui = 0; ui < len; ui++)
    {
        if (!isBinary(data[ui]))
            ret += reinterpret_cast<const char *>(data)[ui];
        else
        {
            char encodedChar[6];
            snprintf(encodedChar, sizeof(encodedChar), "\\\\%03o", data[ui]);
            ret += encodedChar;
        }
    }

    ret += "'::bytea";

    return ret;
}

