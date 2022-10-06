#include <fluffycoin/block/Block.h>
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
#if 0
        Node *node;
#endif
    } u;
} Block;

ASN1_CHOICE(Block) =
{
    ASN1_SIMPLE(Block, u.null, ASN1_NULL)
  , ASN1_IMP(Block, u.genesis, Genesis, 1)
  , ASN1_IMP(Block, u.reconciliation, Reconciliation, 2)
#if 0
  , ASN1_IMP(Block, u.node, Node, 3)
#endif
} ASN1_CHOICE_END(Block)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

Block::Block()
{
    this->type = Type::None;
}

Block::Block(Block &&rhs)
{
    operator=(std::move(rhs));
}

Block &Block::operator=(Block &&rhs)
{
    if (this != &rhs)
    {
        this->type = rhs.type;
        rhs.type = Type::None;

        this->genesis = std::move(rhs.genesis);
        this->reconciliation = std::move(rhs.reconciliation);
#if 0
        this->node = std::move(rhs.node);
#endif
    }

    return (*this);
}

Block::Type Block::getType() const
{
    return type;
}

void Block::setType(Type type)
{
    this->type = type;
}

const std::unique_ptr<Genesis> &Block::getGenesis() const
{
    return genesis;
}

void Block::setGenesis(Genesis genesis)
{
    type = Type::Genesis;
    this->genesis = std::make_unique<Genesis>(std::move(genesis));
    reconciliation.reset();
#if 0
    node.reset();
#endif
}

const std::unique_ptr<Reconciliation> &Block::getReconciliation() const
{
    return reconciliation;
}

void Block::setReconciliation(Reconciliation reconciliation)
{
    type = Type::Reconciliation;
    genesis.reset();
    this->reconciliation = std::make_unique<Reconciliation>(std::move(reconciliation));
#if 0
    node.reset();
#endif
}

#if 0
const std::unique_ptr<Node> &Block::getNode() const
{
    return node;
}

void Block::setNode(Node node)
{
    type = Type::Node;
    genesis.reset();
    reconciliation.reset();
    this->node = std::make_unique<Node>(std::move(node));
}
#endif

void Block::toASN1(asn1::Block &t) const
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
#if 0
            asn1::Node_free(t.u.node);
            t.u.node = nullptr;
#endif
            break;
        default:

            if (t.u.null)
                log::error("Invalid toASN1 cleanup block type {}.", t.type);
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
            genesis->toASN1(*t.u.genesis);
            break;
        case Type::Reconciliation:
            t.u.reconciliation = asn1::Reconciliation_new();
            reconciliation->toASN1(*t.u.reconciliation);
            break;
        case Type::Node:
#if 0
            t.u.node = asn1::Node_new();
            node->toASN1(*t.u.node);
#endif
            break;
        default:
            log::error("Invalid toASN1 block type {}.", static_cast<int>(type));
            break;
    }
}

void Block::fromASN1(const asn1::Block &t)
{
    (*this) = Block();
    switch (static_cast<Block::Type>(t.type))
    {
        case Type::None:
            break;
        case Type::Genesis:
            type = Type::Genesis;
            genesis = std::make_unique<Genesis>();
            genesis->fromASN1(*t.u.genesis);
            break;
        case Type::Reconciliation:
            type = Type::Reconciliation;
            reconciliation = std::make_unique<Reconciliation>();
            reconciliation->fromASN1(*t.u.reconciliation);
            break;
        case Type::Node:
#if 0
            type = Type::Node;
            node = std::make_unique<Node>();
            node->fromASN1(*t.u.node);
#endif
            break;
        default:
            log::error("Invalid fromASN1 block type {}.", t.type);
            break;
    }
}
