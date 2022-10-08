#pragma once

#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/ossl/convert.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Hash_st Hash;
    DECLARE_ASN1_FUNCTIONS(Hash)
    SKM_DEFINE_STACK_OF(Hash, Hash, Hash)
    FLUFFYCOIN_DEFINE_STACK_FUNCTIONS(Hash)
}

namespace block
{

/**
 * Collission proof hash that combines SHA-2 and SHA-3
 */
class Hash
{
    public:
        Hash() = default;
        Hash(Hash &&) = default;
        Hash(const Hash &) = default;
        Hash &operator=(Hash &&) = default;
        Hash &operator=(const Hash &) = default;
        ~Hash() = default;

        const BinData &getSha2() const;
        void setSha2(BinData hash);

        const BinData &getSha3() const;
        void setSha3(BinData hash);

        void toASN1(asn1::Hash &) const;
        void fromASN1(const asn1::Hash &);

        bool operator==(const Hash &rhs) const;
        bool operator!=(const Hash &rhs) const;

    private:
        BinData sha2;
        BinData sha3;
};

}

}
