#include <fluffycoin/block/Node.h>

#include <fluffycoin/ossl/convert.h>
#include <fluffycoin/ossl/encode.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct NodeContent_st
{
    ASN1_INTEGER *chainId;
    ASN1_INTEGER *shardId;
    ASN1_INTEGER *index;
    Hash *prevHash;
    STACK_OF(Transaction) *transactions;
    ASN1_OCTET_STRING *nonce;
    ASN1_OCTET_STRING *leader;
    ASN1_OCTET_STRING *newAddress;
} NodeContent;

DECLARE_ASN1_FUNCTIONS(NodeContent)

ASN1_SEQUENCE(NodeContent) =
{
    ASN1_SIMPLE(NodeContent, chainId, ASN1_INTEGER)
  , ASN1_SIMPLE(NodeContent, shardId, ASN1_INTEGER)
  , ASN1_SIMPLE(NodeContent, index, ASN1_INTEGER)
  , ASN1_SIMPLE(NodeContent, prevHash, Hash)
  , ASN1_SEQUENCE_OF(NodeContent, transactions, Transaction)
  , ASN1_SIMPLE(NodeContent, nonce, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(NodeContent, leader, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(NodeContent, newAddress, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(NodeContent)

IMPLEMENT_ASN1_FUNCTIONS(NodeContent)

typedef struct Node_st
{
    NodeContent *content;
    ASN1_OCTET_STRING *signature;
    STACK_OF(Validation) *votes;
} Node;

ASN1_SEQUENCE(Node) =
{
    ASN1_SIMPLE(Node, content, NodeContent)
  , ASN1_SIMPLE(Node, signature, ASN1_OCTET_STRING)
  , ASN1_SET_OF(Node, votes, Validation)
} ASN1_SEQUENCE_END(Node)

IMPLEMENT_ASN1_FUNCTIONS(Node)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

Node::Node()
{
    chainId = 0;
    shardId = 0;
    index = 0;
}

uint64_t Node::getChainId() const
{
    return chainId;
}

void Node::setChainId(uint64_t chainId)
{
    this->chainId = chainId;
}

uint32_t Node::getShardId() const
{
    return shardId;
}

void Node::setShardId(uint32_t shardId)
{
    this->shardId = shardId;
}

uint32_t Node::getIndex() const
{
    return index;
}

void Node::setIndex(uint32_t index)
{
    this->index = index;
}

const Hash &Node::getPrevHash() const
{
    return prevHash;
}

void Node::setPrevHash(Hash hash)
{
    this->prevHash = std::move(hash);
}

const std::list<Transaction> &Node::getTransactions() const
{
    return transactions;
}

void Node::setTransactions(std::list<Transaction> transactions)
{
    this->transactions = std::move(transactions);
}

const BinData &Node::getNonce() const
{
    return nonce;
}

void Node::setNonce(BinData nonce)
{
    this->nonce = std::move(nonce);
}

const Address &Node::getLeader() const
{
    return leader;
}

void Node::setLeader(Address address)
{
    this->leader = std::move(address);
}

const Address &Node::getNewAddress() const
{
    return newAddress;
}

void Node::setNewAddress(Address address)
{
    this->newAddress = std::move(address);
}

const Signature &Node::getSignature() const
{
    return signature;
}

void Node::setSignature(Signature sig)
{
    this->signature = std::move(sig);
}

const std::list<Validation> &Node::getVotes() const
{
    return votes;
}

void Node::setVotes(std::list<Validation> votes)
{
    this->votes = std::move(votes);
}

void Node::toAsn1(asn1::Node &t) const
{
    signature.toAsn1(*t.signature);
    asn1::toValidationStack(*t.votes, votes);

    ossl::fromUInt64(*t.content->chainId, chainId);
    ossl::fromUInt32(*t.content->shardId, shardId);
    ossl::fromUInt32(*t.content->index, index);
    prevHash.toAsn1(*t.content->prevHash);
    asn1::toTransactionStack(*t.content->transactions, transactions);
    ossl::fromBin(*t.content->nonce, nonce);
    leader.toAsn1(*t.content->leader);
    newAddress.toAsn1(*t.content->newAddress);
}

void Node::fromAsn1(const asn1::Node &t)
{
    signature.fromAsn1(*t.signature);
    asn1::fromValidationStack(votes, *t.votes);

    chainId = ossl::toUInt64(*t.content->chainId);
    shardId = ossl::toUInt32(*t.content->shardId);
    index = ossl::toUInt32(*t.content->index);
    prevHash.fromAsn1(*t.content->prevHash);
    asn1::fromTransactionStack(transactions, *t.content->transactions);
    nonce = ossl::toBin(*t.content->nonce);
    leader.fromAsn1(*t.content->leader);
    newAddress.fromAsn1(*t.content->newAddress);
}

BinData Node::toContent() const
{
    asn1::Node *obj = asn1::Node_new();
    toAsn1(*obj);
    BinData data = ossl::encode(*obj->content, asn1::i2d_NodeContent);
    asn1::Node_free(obj);
    return data;
}
