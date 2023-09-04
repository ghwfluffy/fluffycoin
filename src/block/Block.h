#pragma once

#include <fluffycoin/block/Genesis.h>
#include <fluffycoin/block/Reconciliation.h>
#include <fluffycoin/block/Node.h>
#include <fluffycoin/utils/Strings.h>

#include <openssl/asn1.h>

#include <memory>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Block_st Block;
    DECLARE_ASN1_FUNCTIONS(Block)
}

namespace block
{

/**
 * A block in the block chain
 */
class Block
{
    public:
        Block();
        Block(Block &&);
        Block(const Block &) = delete;
        Block &operator=(Block &&);
        Block &operator=(const Block &) = delete;
        ~Block() = default;

        enum class Type : int
        {
            None = 0,
            Genesis,
            Reconciliation,
            Node,
        };

        Type getType() const;

        const std::unique_ptr<Genesis> &getGenesis() const;
        void setGenesis(Genesis genesis);

        const std::unique_ptr<Reconciliation> &getReconciliation() const;
        void setReconciliation(Reconciliation reconciliation);

        const std::unique_ptr<Node> &getNode() const;
        void setNode(Node node);

        void toAsn1(asn1::Block &) const;
        void fromAsn1(const asn1::Block &);

        BinData encode() const;

    private:
        void resetChoice();

        Type type;
        std::unique_ptr<Genesis> genesis;
        std::unique_ptr<Reconciliation> reconciliation;
        std::unique_ptr<Node> node;
};

}

std::string to_string(block::Block::Type);

}
