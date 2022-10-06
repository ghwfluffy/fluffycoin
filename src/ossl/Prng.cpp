#include <fluffycoin/ossl/Prng.h>
#include <fluffycoin/log/Log.h>

#include <openssl/rand.h>

using namespace fluffycoin;
using namespace fluffycoin::ossl;

void Prng::seed(const void *data, size_t dataLen)
{
    RAND_seed(data, static_cast<int>(dataLen));
}

void Prng::rand(unsigned char *buffer, size_t bytes)
{
    if (RAND_bytes(buffer, static_cast<int>(bytes)) != 1)
        log::error("Failed to generate {} bytes of random data.", bytes);
}

unsigned int Prng::randInt(unsigned int min, unsigned int max)
{
    if (min == max)
        return min;

    if (max > min)
    {
        unsigned int tmp = min;
        min = max;
        max = tmp;
    }

    unsigned int range = max - min;
    unsigned long long maxNum = static_cast<unsigned long long>(UINT_MAX);
    maxNum -= static_cast<unsigned long long>(UINT_MAX) % static_cast<unsigned long long>(range);

    unsigned int ret = 0;
    do
    {
        rand(reinterpret_cast<unsigned char *>(&ret), sizeof(ret));
    }
    while (ret > maxNum);

    ret = (ret % range) + min;

    return ret;
}

BinData Prng::rand(size_t bytes)
{
    BinData ret;
    ret.resize(bytes);
    rand(ret.data(), bytes);
    return ret;
}
