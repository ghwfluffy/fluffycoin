#pragma once

#include <fluffycoin/ossl/EvpPkeyPtr.h>

#include <fluffycoin/utils/SafeData.h>

namespace fluffycoin::p2p
{

/**
 * The key associated with the stake address of this p2p node
 */
class StakeKey
{
    public:
        StakeKey() = default;
        StakeKey(StakeKey &&) = default;
        StakeKey(const StakeKey &) = delete;
        StakeKey &operator=(StakeKey &&) = default;
        StakeKey &operator=(const StakeKey &) = delete;
        ~StakeKey() = default;

        const BinData &getPub() const;
        const SafeData &getPriv() const;

        const ossl::EvpPkeyPtr &getKey() const;

        const std::string &getAddress() const;

        void setKey(
            std::string address,
            ossl::EvpPkeyPtr key);

    private:
        std::string address;
        ossl::EvpPkeyPtr key;
        BinData pub;
        SafeData priv;
};

}
