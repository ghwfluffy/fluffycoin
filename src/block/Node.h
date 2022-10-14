#pragma once

#include <fluffycoin/block/Hash.h>
#include <fluffycoin/block/Address.h>
#include <fluffycoin/block/Signature.h>
#include <fluffycoin/block/Validation.h>
#include <fluffycoin/block/Transaction.h>

#include <fluffycoin/utils/BinData.h>

#include <list>

#include <stdint.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Node_st Node;
    DECLARE_ASN1_FUNCTIONS(Node)
}

namespace block
{

/**
 * The most common block in the blockchain. A bunch of transactions
 * are combined into a node and appended to one of the active
 * blockchain shards.
 */
class Node
{
    public:
        Node();
        Node(Node &&) = default;
        Node(const Node &) = default;
        Node &operator=(Node &&) = default;
        Node &operator=(const Node &) = default;
        ~Node() = default;

        uint64_t getChainId() const;
        void setChainId(uint64_t chainId);

        uint32_t getShardId() const;
        void setShardId(uint32_t shardId);

        uint32_t getIndex() const;
        void setIndex(uint32_t index);

        const Hash &getPrevHash() const;
        void setPrevHash(Hash hash);

        const std::list<Transaction> &getTransactions() const;
        void setTransactions(std::list<Transaction> transactions);

        const BinData &getNonce() const;
        void setNonce(BinData nonce);

        const Address &getLeader() const;
        void setLeader(Address leader);

        const Address &getNewAddress() const;
        void setNewAddress(Address address);

        const Signature &getSignature() const;
        void setSignature(Signature sig);

        const std::list<Validation> &getVotes() const;
        void setVotes(std::list<Validation> votes);

        void toAsn1(asn1::Node &) const;
        void fromAsn1(const asn1::Node &);

        BinData toContent() const;

    private:
        uint64_t chainId;
        uint32_t shardId;
        uint32_t index;

        Hash prevHash;
        std::list<Transaction> transactions;

        BinData nonce;
        Address leader;
        Address newAddress;
        Signature signature;

        std::list<Validation> votes;
};

}

}
