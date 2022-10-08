#include <fluffycoin/block/Reconciliation.h>
#include <fluffycoin/block/Time.h>

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
    ASN1_OCTET_STRING *newAddress;
} ReconciliationContent;
DECLARE_ASN1_FUNCTIONS(ReconciliationContent);

ASN1_SEQUENCE(ReconciliationContent) =
{
    ASN1_SIMPLE(ReconciliationContent, protocol, ASN1_INTEGER)
  , ASN1_SIMPLE(ReconciliationContent, chainId, ASN1_INTEGER)
  , ASN1_SEQUENCE_OF(ReconciliationContent, shardHashes, Hash)
  , ASN1_SIMPLE(ReconciliationContent, leader, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(ReconciliationContent, newAddress, ASN1_OCTET_STRING)
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
    ossl::fromUInt32(*t.content->protocol, protocol);
    ossl::fromUInt64(*t.content->chainId, chainId);
    asn1::toHashStack(*t.content->shardHashes, shardHashes);
    ossl::fromBin(*t.content->leader, leader);
    ossl::fromBin(*t.content->newAddress, newAddress);
    ossl::fromBin(*t.signature, signature);
    asn1::toValidationStack(*t.votes, votes);
}

void Reconciliation::fromASN1(const asn1::Reconciliation &t)
{
    protocol = ossl::toUInt32(*t.content->protocol);
    chainId = ossl::toUInt64(*t.content->chainId);
    asn1::fromHashStack(shardHashes, *t.content->shardHashes);
    leader = ossl::toBin(*t.content->leader);
    newAddress = ossl::toBin(*t.content->newAddress);
    signature = ossl::toBin(*t.signature);
    asn1::fromValidationStack(votes, *t.votes);
}

BinData Reconciliation::toContent() const
{
    asn1::Reconciliation *obj = asn1::Reconciliation_new();
    toASN1(*obj);

    int len = i2d_ReconciliationContent(obj->content, nullptr);
    BinData data;
    data.resize(static_cast<size_t>(len));

    unsigned char *pauc = data.data();
    i2d_ReconciliationContent(obj->content, &pauc);

    asn1::Reconciliation_free(obj);

    return data;
}
