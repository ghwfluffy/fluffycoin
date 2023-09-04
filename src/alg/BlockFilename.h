#pragma once

#include <string>

#include <stdint.h>

namespace fluffycoin::alg
{

/**
 * Generate or parse the expected filename for a serialized DER block
 */
namespace BlockFilename
{
    std::string get(
        uint32_t reconciliation,
        uint32_t shard,
        uint32_t block);

    bool parse(
        const std::string &str,
        uint32_t &reconciliation,
        uint32_t &shard,
        uint32_t &block);
}

}
