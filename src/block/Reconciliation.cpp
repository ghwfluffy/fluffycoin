#include <fluffycoin/block/Reconciliation.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Reconciliation_t
{
    ASN1_INTEGER *chainId;
    STACK_OF(Hash) *shardHashes;
    ASN1_OCTET_STRING *leader;
    ASN1_OCTET_STRING *signature;
    STACK_OF(Validation) *votes;
} Reconciliation;

ASN1_SEQUENCE(Reconciliation) =
{
    ASN1_SIMPLE(Reconciliation, chainId, ASN1_INTEGER)
  , ASN1_SEQUENCE_OF(Reconciliation, shardHashes, Hash)
  , ASN1_SIMPLE(Reconciliation, leader, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(Reconciliation, signature, ASN1_OCTET_STRING)
  , ASN1_SET_OF(Reconciliation, votes, Validation)
} ASN1_SEQUENCE_END(Reconciliation)

IMPLEMENT_ASN1_FUNCTIONS(Reconciliation)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

Reconciliation::Reconciliation()
{
    chainId = 0;
}

uint64_t Reconciliation::getChainId() const
{
    return chainId;
}

void Reconciliation::setChainId(uint64_t chainId)
{
    this->chainId = chainId;
}

const std::list<Hash> &Reconciliation::getShardHashes() const
{
    return shardHashes;
}

void Reconciliation::setShardHashes(std::list<Hash> hashes)
{
    this->shardHashes = std::move(hashes);
}

const BinData &Reconciliation::getLeader() const
{
    return leader;
}

void Reconciliation::setLeader(BinData leader)
{
    this->leader = std::move(leader);
}

const BinData &Reconciliation::getSignature() const
{
    return signature;
}

void Reconciliation::setSignature(BinData signature)
{
    this->signature = std::move(signature);
}

const std::list<Validation> &Reconciliation::getVotes() const
{
    return votes;
}

void Reconciliation::setVotes(std::list<Validation> votes)
{
    this->votes = std::move(votes);
}

void Reconciliation::toASN1(asn1::Reconciliation &t) const
{
    ASN1_INTEGER_set_uint64(t.chainId, chainId);

    asn1::sk_Hash_pop_free(t.shardHashes, asn1::Hash_free);
    for (const Hash &cur : shardHashes)
    {
        asn1::Hash *pt = asn1::Hash_new();
        cur.toASN1(*pt);
        asn1::sk_Hash_push(t.shardHashes, pt);
    }

    ASN1_STRING_set(t.leader, leader.data(), static_cast<int>(leader.length()));
    ASN1_STRING_set(t.signature, signature.data(), static_cast<int>(signature.length()));

    asn1::sk_Validation_pop_free(t.votes, asn1::Validation_free);
    for (const Validation &cur : votes)
    {
        asn1::Validation *pt = asn1::Validation_new();
        cur.toASN1(*pt);
        asn1::sk_Validation_push(t.votes, pt);
    }
}

void Reconciliation::fromASN1(const asn1::Reconciliation &t)
{
    chainId = static_cast<uint64_t>(ASN1_INTEGER_get(t.chainId));

    shardHashes.clear();
    for (int i = 0; i < asn1::sk_Hash_num(t.shardHashes); i++)
    {
        asn1::Hash *pt = asn1::sk_Hash_value(t.shardHashes, i);
        Hash cur;
        cur.fromASN1(*pt);
        shardHashes.emplace_back(std::move(cur));
    }

    leader.setData(ASN1_STRING_get0_data(t.leader), static_cast<size_t>(ASN1_STRING_length(t.leader)));
    signature.setData(ASN1_STRING_get0_data(t.signature), static_cast<size_t>(ASN1_STRING_length(t.signature)));

    votes.clear();
    for (int i = 0; i < asn1::sk_Validation_num(t.votes); i++)
    {
        asn1::Validation *pt = asn1::sk_Validation_value(t.votes, i);
        Validation cur;
        cur.fromASN1(*pt);
        votes.emplace_back(std::move(cur));
    }
}
