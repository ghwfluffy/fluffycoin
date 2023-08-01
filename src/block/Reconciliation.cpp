#include <fluffycoin/block/Reconciliation.h>
#include <fluffycoin/block/Time.h>

#include <fluffycoin/ossl/convert.h>
#include <fluffycoin/ossl/encode.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct ReconciliationContent_t
{
    ASN1_INTEGER *protocol;
    Time *time;
    ASN1_INTEGER *chainId;
    STACK_OF(Hash) *shardHashes;
    ASN1_OCTET_STRING *leader;
} ReconciliationContent;

DECLARE_ASN1_FUNCTIONS(ReconciliationContent)

ASN1_SEQUENCE(ReconciliationContent) =
{
    ASN1_SIMPLE(ReconciliationContent, protocol, ASN1_INTEGER)
  , ASN1_SIMPLE(ReconciliationContent, time, ASN1_INTEGER)
  , ASN1_SIMPLE(ReconciliationContent, chainId, ASN1_INTEGER)
  , ASN1_SEQUENCE_OF(ReconciliationContent, shardHashes, Hash)
  , ASN1_SIMPLE(ReconciliationContent, leader, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(ReconciliationContent)

IMPLEMENT_ASN1_FUNCTIONS(ReconciliationContent)


typedef struct Reconciliation_t
{
    ReconciliationContent *content;
    ASN1_OCTET_STRING *signature;
    STACK_OF(Validation) *votes;
} Reconciliation;

ASN1_SEQUENCE(Reconciliation) =
{
    ASN1_SIMPLE(Reconciliation, content, ReconciliationContent)
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
    protocol = 0;
}

uint32_t Reconciliation::getProtocol() const
{
    return protocol;
}

void Reconciliation::setProtocol(uint32_t protocol)
{
    this->protocol = protocol;
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

const Address &Reconciliation::getLeader() const
{
    return leader;
}

void Reconciliation::setLeader(Address leader)
{
    this->leader = std::move(leader);
}

const Signature &Reconciliation::getSignature() const
{
    return signature;
}

void Reconciliation::setSignature(Signature signature)
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

void Reconciliation::toAsn1(asn1::Reconciliation &t) const
{
    ossl::fromUInt32(*t.content->protocol, protocol);
    ossl::fromUInt64(*t.content->chainId, chainId);
    asn1::toHashStack(*t.content->shardHashes, shardHashes);
    leader.toAsn1(*t.content->leader);
    signature.toAsn1(*t.signature);
    asn1::toValidationStack(*t.votes, votes);
}

void Reconciliation::fromAsn1(const asn1::Reconciliation &t)
{
    protocol = ossl::toUInt32(*t.content->protocol);
    chainId = ossl::toUInt64(*t.content->chainId);
    asn1::fromHashStack(shardHashes, *t.content->shardHashes);
    leader.fromAsn1(*t.content->leader);
    signature.fromAsn1(*t.signature);
    asn1::fromValidationStack(votes, *t.votes);
}

BinData Reconciliation::toContent() const
{
    asn1::Reconciliation *obj = asn1::Reconciliation_new();
    toAsn1(*obj);
    BinData data = ossl::encode(*obj->content, asn1::i2d_ReconciliationContent);
    asn1::Reconciliation_free(obj);
    return data;
}

BinData Reconciliation::encode() const
{
    asn1::Reconciliation *obj = asn1::Reconciliation_new();
    toAsn1(*obj);
    BinData data = ossl::encode(*obj, asn1::i2d_Reconciliation);
    asn1::Reconciliation_free(obj);
    return data;
}
