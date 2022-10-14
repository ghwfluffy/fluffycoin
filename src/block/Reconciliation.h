#pragma once

#include <fluffycoin/block/Validation.h>
#include <fluffycoin/block/Hash.h>

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

#include <list>
#include <stdint.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Reconciliation_t Reconciliation;
    DECLARE_ASN1_FUNCTIONS(Reconciliation);
}

namespace block
{

/**
 * Periodic block that combines all the shards before sharding again
 */
class Reconciliation
{
    public:
        Reconciliation();
        Reconciliation(Reconciliation &&) = default;
        Reconciliation(const Reconciliation &) = default;
        Reconciliation &operator=(Reconciliation &&) = default;
        Reconciliation &operator=(const Reconciliation &) = default;
        ~Reconciliation() = default;

        uint32_t getProtocol() const;
        void setProtocol(uint32_t protocol);

        uint64_t getChainId() const;
        void setChainId(uint64_t chainId);

        const std::list<Hash> &getShardHashes() const;
        void setShardHashes(std::list<Hash> hashes);

        const BinData &getLeader() const;
        void setLeader(BinData leader);

        const BinData &getSignature() const;
        void setSignature(BinData signature);

        const std::list<Validation> &getVotes() const;
        void setVotes(std::list<Validation> votes);

        void toAsn1(asn1::Reconciliation &) const;
        void fromAsn1(const asn1::Reconciliation &);

        BinData toContent() const;

    private:
        uint32_t protocol;
        uint64_t chainId;
        std::list<Hash> shardHashes;
        BinData leader;
        BinData newAddress;
        BinData signature;
        std::list<Validation> votes;
};

}

}
