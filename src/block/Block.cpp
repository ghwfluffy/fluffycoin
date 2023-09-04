#include <fluffycoin/block/Block.h>
#include <fluffycoin/ossl/encode.h>
#include <fluffycoin/log/Log.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Block_st
{
    int type;
    union {
        ASN1_NULL *null;
        Genesis *genesis;
        Reconciliation *reconciliation;
        Node *node;
    } u;
} Block;

ASN1_CHOICE(Block) =
{
    ASN1_SIMPLE(Block, u.null, ASN1_NULL)
  , ASN1_IMP(Block, u.genesis, Genesis, 1)
  , ASN1_IMP(Block, u.reconciliation, Reconciliation, 2)
  , ASN1_IMP(Block, u.node, Node, 3)
} ASN1_CHOICE_END(Block)

IMPLEMENT_ASN1_FUNCTIONS(Block)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

Block::Block()
{
    resetChoice();
}

Block::Block(Block &&rhs)
{
    resetChoice();
    operator=(std::move(rhs));
}

Block::Block(const Block &rhs)
{
    resetChoice();
    operator=(rhs);
}

Block &Block::operator=(Block &&rhs)
{
    if (this != &rhs)
    {
        resetChoice();

        this->type = rhs.type;
        this->genesis = std::move(rhs.genesis);
        this->reconciliation = std::move(rhs.reconciliation);
        this->node = std::move(rhs.node);

        rhs.resetChoice();
    }

    return (*this);
}

Block &Block::operator=(const Block &rhs)
{
    if (this != &rhs)
    {
        asn1::Block t = {};
        rhs.toAsn1(t);
        fromAsn1(t);
    }

    return (*this);
}

void Block::resetChoice()
{
    type = Type::None;

    genesis.reset();
    reconciliation.reset();
    node.reset();
}

Block::Type Block::getType() const
{
    return type;
}

const std::unique_ptr<Genesis> &Block::getGenesis() const
{
    return genesis;
}

void Block::setGenesis(Genesis genesis)
{
    resetChoice();
    type = Type::Genesis;
    this->genesis = std::make_unique<Genesis>(std::move(genesis));
}

const std::unique_ptr<Reconciliation> &Block::getReconciliation() const
{
    return reconciliation;
}

void Block::setReconciliation(Reconciliation reconciliation)
{
    resetChoice();
    type = Type::Reconciliation;
    this->reconciliation = std::make_unique<Reconciliation>(std::move(reconciliation));
}

const std::unique_ptr<Node> &Block::getNode() const
{
    return node;
}

void Block::setNode(Node node)
{
    resetChoice();
    type = Type::Node;
    this->node = std::make_unique<Node>(std::move(node));
}

void Block::toAsn1(asn1::Block &t) const
{
    // Cleanup any current memory
    switch (static_cast<Block::Type>(t.type))
    {
        case Type::None:
            ASN1_NULL_free(t.u.null);
            t.u.null = nullptr;
            break;
        case Type::Genesis:
            asn1::Genesis_free(t.u.genesis);
            t.u.genesis = nullptr;
            break;
        case Type::Reconciliation:
            asn1::Reconciliation_free(t.u.reconciliation);
            t.u.reconciliation = nullptr;
            break;
        case Type::Node:
            asn1::Node_free(t.u.node);
            t.u.node = nullptr;
            break;
        default:
            if (t.u.null)
                log::error("Invalid toAsn1 cleanup block type {}.", t.type);
            break;
    }

    // Set new memory
    t.type = static_cast<int>(type);
    switch (type)
    {
        case Type::None:
            t.u.null = ASN1_NULL_new();
            break;
        case Type::Genesis:
            t.u.genesis = asn1::Genesis_new();
            genesis->toAsn1(*t.u.genesis);
            break;
        case Type::Reconciliation:
            t.u.reconciliation = asn1::Reconciliation_new();
            reconciliation->toAsn1(*t.u.reconciliation);
            break;
        case Type::Node:
            t.u.node = asn1::Node_new();
            node->toAsn1(*t.u.node);
            break;
        default:
            log::error("Invalid toAsn1 block type {}.", static_cast<int>(type));
            break;
    }
}

void Block::fromAsn1(const asn1::Block &t)
{
    resetChoice();
    switch (static_cast<Block::Type>(t.type))
    {
        case Type::None:
            break;
        case Type::Genesis:
            type = Type::Genesis;
            genesis = std::make_unique<Genesis>();
            genesis->fromAsn1(*t.u.genesis);
            break;
        case Type::Reconciliation:
            type = Type::Reconciliation;
            reconciliation = std::make_unique<Reconciliation>();
            reconciliation->fromAsn1(*t.u.reconciliation);
            break;
        case Type::Node:
            type = Type::Node;
            node = std::make_unique<Node>();
            node->fromAsn1(*t.u.node);
            break;
        default:
            log::error("Invalid fromAsn1 block type {}.", t.type);
            break;
    }
}

BinData Block::encode() const
{
    asn1::Block *obj = asn1::Block_new();
    toAsn1(*obj);
    BinData data = ossl::encode(*obj, asn1::i2d_Block);
    asn1::Block_free(obj);
    return data;
}

std::string fluffycoin::to_string(block::Block::Type type)
{
    switch (type)
    {
        case block::Block::Type::Genesis:
            return "Genesis";
        case block::Block::Type::Reconciliation:
            return "Reconciliation";
        case block::Block::Type::Node:
            return "Node";
        default:
        case block::Block::Type::None:
            break;
    }

    return "None";
}
