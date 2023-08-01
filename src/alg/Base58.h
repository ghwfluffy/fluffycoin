#pragma once

#include <fluffycoin/utils/BinData.h>

#include <string>

namespace fluffycoin::alg
{

/**
 * Encode and Decode Base58 data
 */
namespace Base58
{
    std::string encode(const BinData &binary);
    BinData decode(const std::string &printable);

    unsigned char charToVal(char curChar);
    char valToChar(unsigned char val);
}

}
