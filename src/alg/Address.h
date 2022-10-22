#pragma once

#include <fluffycoin/utils/BinData.h>

#include <openssl/evp.h>

#include <string>

namespace fluffycoin
{

namespace alg
{

namespace Address
{
    constexpr const size_t LENGTH = 26;
    constexpr const size_t SEED_LENGTH = 6;

    BinData generate(const EVP_PKEY &key);
    BinData generate(const EVP_PKEY &key, const char *seed);
    BinData generate(const EVP_PKEY &key, const BinData &seed);
    BinData generate(const EVP_PKEY &key, const std::string &printableSeed);
    bool verify(const BinData &address, const EVP_PKEY &key);

    std::string printable(const BinData &address);
    BinData unprintable(const std::string &address);
}

}

}
