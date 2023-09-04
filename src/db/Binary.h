#pragma once

#include <fluffycoin/utils/BinData.h>

#include <string>

namespace fluffycoin::db
{

namespace Binary
{
    std::string encode(
        const BinData &data);

    std::string encode(
        const unsigned char *data,
        size_t len);
}

}
