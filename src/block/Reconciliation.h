#pragma once

#include <fluffycoin/block/Validation.h>
#include <fluffycoin/block/Hash.h>
#include <fluffycoin/block/Address.h>
#include <fluffycoin/block/Signature.h>

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

#include <list>
#include <vector>
#include <stdint.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Reconciliation_t Reconciliation;
    DECLARE_ASN1_FUNCTIONS(Reconciliation);

    typedef struct ReconciliationShardInfo_t ReconciliationShardInfo;
    DECLARE_ASN1_FUNCTIONS(ReconciliationShardInfo);
}

namespace block
{

/**
 * Information about one of the shards being combined into the reconciliation block
 */
struct ReconciliationShardInfo
{
    Hash hash;
    uint32_t blocks = 0;

    ReconciliationShardInfo() = default;
    ReconciliationShardInfo(Hash hash, uint32_t blocks);
    ReconciliationShardInfo(ReconciliationShardInfo &&) = default;
    ReconciliationShardInfo(const ReconciliationShardInfo &) = default;
    ReconciliationShardInfo &operator=(ReconciliationShardInfo &&) = default;
    ReconciliationShardInfo &operator=(const ReconciliationShardInfo &) = default;
    ~ReconciliationShardInfo() = default;

    void toAsn1(asn1::ReconciliationShardInfo &) const;
    void fromAsn1(const asn1::ReconciliationShardInfo &);
};

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

        const std::vector<ReconciliationShardInfo> &getShardInfo() const;
        void setShardInfo(std::vector<ReconciliationShardInfo> hashes);

        const Address &getLeader() const;
        void setLeader(Address leader);

        const Signature &getSignature() const;
        void setSignature(Signature signature);

        const std::list<Validation> &getVotes() const;
        void setVotes(std::list<Validation> votes);

        void toAsn1(asn1::Reconciliation &) const;
        void fromAsn1(const asn1::Reconciliation &);

        BinData toContent() const;
        BinData encode() const;

    private:
        uint32_t protocol;
        uint64_t chainId;
        std::vector<ReconciliationShardInfo> shardInfo;
        Address leader;
        Signature signature;
        std::list<Validation> votes;
};

}

}
