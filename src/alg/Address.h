#pragma once

#include <fluffycoin/utils/BinData.h>

#include <openssl/evp.h>

#include <string>

namespace fluffycoin::alg
{

/**
 * Utilities for generating/matching Fluffycoin wallet/validator addresses
 */
namespace Address
{
    constexpr const size_t LENGTH = 26;
    constexpr const size_t SALT_LENGTH = 6;

    BinData generate(const EVP_PKEY &key);
    BinData generate(const EVP_PKEY &key, const char *salt);
    BinData generate(const EVP_PKEY &key, const BinData &salt);
    BinData generate(const EVP_PKEY &key, const std::string &printableSalt);

    bool verify(const BinData &address, const EVP_PKEY &key);
    bool verify(const std::string &address, const EVP_PKEY &key);

    std::string printable(const BinData &address);
    BinData unprintable(const std::string &address);
}

}
