#pragma once

#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/utils/Details.h>

namespace fluffycoin::p2p
{

// TODO: This comes from the peer info microservice
struct ValidatorInfo
{
    const BinData &getPubKey() const;
    bool isActive() const;
};

/**
 * Resolve a peer's address into the information about that peer
 */
class ValidatorLookup
{
    public:
        ValidatorLookup() = default;
        ValidatorLookup(ValidatorLookup &&) = default;
        ValidatorLookup(const ValidatorLookup &) = default;
        ValidatorLookup &operator=(ValidatorLookup &&) = default;
        ValidatorLookup &operator=(const ValidatorLookup &) = default;
        ~ValidatorLookup() = default;

        ValidatorInfo getValidator(
            const std::string &address,
            Details &details);
};

}
