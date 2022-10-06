#pragma once

#include <fluffycoin/utils/BinData.h>

namespace fluffycoin
{

namespace ossl
{

/**
 * Generate random data
 */
namespace Prng
{
    void seed(const void *data, size_t dataLen);

    BinData rand(size_t bytes);
    void rand(unsigned char *buffer, size_t bytes);
    unsigned int randInt(unsigned int min, unsigned int max);
}

}

}
