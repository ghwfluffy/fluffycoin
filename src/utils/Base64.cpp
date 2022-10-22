#include <fluffycoin/utils/Base64.h>
#include <fluffycoin/log/Log.h>

using namespace fluffycoin;

/**
 * Convert a Base64 character to the associated integer value.
 * Supports all Base64 encoding schemes.
 *
 * @param curChar Base64 character
 *
 * @return Associated integer value, -1 if not a valid Base64 character
 */
char Base64::charToVal(char curChar)
{
    char ret = -1;
    if (curChar >= 'A' && curChar <= 'Z')
        ret = curChar - 'A';
    else if (curChar >= 'a' && curChar <= 'z')
        ret = curChar - 'a' + 26;
    else if (curChar >= '0' && curChar <= '9')
        ret = curChar - '0' + 52;
    else if (curChar == '+' || curChar == '-')
        ret = 62;
    else if (curChar == '/' || curChar == '_' || curChar == ',')
        ret = 63;
    else
        log::error("Invalid Base64 character {}.", curChar);
    return ret;
}

namespace
{

constexpr const char webEncoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
static_assert((sizeof(webEncoding) - sizeof('\0')) == 64, "Web base64 encoding requires 64 characters.");

}

char Base64::valToChar(unsigned char val)
{
    char ret = '\0';
    if (val >= sizeof(webEncoding))
        log::error("Invalid Base64 value {}.", val);
    else
        ret = webEncoding[val];
    return ret;
}

std::string Base64::encode(const BinData &binary)
{
    std::string encoded;
    encoded.reserve((binary.length() * 4 / 3) + 1);

    unsigned char bits = 0;
    unsigned char curVal = 0;
    for (size_t ui = 0; ui < binary.length(); ui++)
    {
        unsigned char uc = binary.data()[ui];
        // Take 6
        // Keep 2
        if (bits == 0)
        {
            unsigned char index = uc >> 2;
            encoded += webEncoding[index];
            curVal = uc & 0x03;
            bits = 2;
        }
        // Take 4
        // Keep 4
        else if (bits == 2)
        {
            curVal <<= 4;
            unsigned char index = curVal | (uc >> 4);
            encoded += webEncoding[index];
            curVal = uc & 0x0F;
            bits = 4;
        }
        // Take 2
        // Take 6
        // Keep 0
        else if (bits == 4)
        {
            // Take 2
            curVal <<= 2;
            unsigned char index = curVal | (uc >> 6);
            encoded += webEncoding[index];
            // Take 6
            index = uc & 0x3F;
            encoded += webEncoding[index];
            bits = 0;
        }
    }

    // Bits left over
    if (bits > 0)
    {
        unsigned int index = static_cast<unsigned int>(curVal) << (6 - bits);
        encoded += webEncoding[index];
    }

    return encoded;
}

BinData Base64::decode(const std::string &printable)
{
    // Don't bother with the allocation for a nullop
    if (printable.empty())
        return BinData();

    size_t decodeLen = 0;
    unsigned char *decoded = reinterpret_cast<unsigned char *>(malloc(((printable.length() / 4) + 1) * 3));

    // Convert to binary
    unsigned char bits = 0;
    unsigned char curVal = 0;
    for (size_t ui = 0; ui < printable.length(); ui++)
    {
        char c = printable[ui];
        char intVal = charToVal(c);
        if (intVal < 0)
            continue;

        // Keep 6 bits
        if (bits == 0)
        {
            curVal = static_cast<unsigned char>(intVal);
            bits = 6;
        }
        // Take all 6 bits
        else if (bits == 2)
        {
            curVal <<= 6;
            curVal |= static_cast<unsigned char>(intVal);
            decoded[decodeLen++] = curVal;
            bits = 0;
        }
        // Take 4 bits
        // Keep 2 bits
        else if (bits == 4)
        {
            curVal <<= 4;
            curVal |= static_cast<unsigned char>(intVal >> 2);
            decoded[decodeLen++] = curVal;
            curVal = intVal & 0x03;
            bits = 2;
        }
        // Take 2 bits
        // Keep 4 bits
        else if (bits == 6)
        {
            curVal <<= 2;
            curVal |= static_cast<unsigned char>(intVal >> 4);
            decoded[decodeLen++] = curVal;
            curVal = intVal & 0x0F;
            bits = 4;
        }
    }

    BinData ret;
    ret.setBuffer(decoded, decodeLen);

    return ret;
}
