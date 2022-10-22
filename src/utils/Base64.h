#pragma once

#include <fluffycoin/utils/BinData.h>

#include <string>

namespace fluffycoin
{

namespace Base64
{
    std::string encode(const BinData &binary);
    BinData decode(const std::string &printable);

    char charToVal(char curChar);
    char valToChar(unsigned char val);
}

}
