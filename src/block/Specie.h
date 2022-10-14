#pragma once

#include <stdint.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Specie_st Specie;
    DECLARE_ASN1_FUNCTIONS(Specie)
}

namespace block
{

/**
 * Describes an amount of fluffycoins
 */
class Specie
{
    public:
        Specie();
        Specie(Specie &&) = default;
        Specie(const Specie &) = default;
        Specie &operator=(Specie &&) = default;
        Specie &operator=(const Specie &) = default;
        ~Specie() = default;

        uint64_t getCoins() const;
        void setCoins(uint64_t coins);

        // 1/100000000 of a coin
        uint32_t getFluffs() const;
        void setFluffs(uint32_t fluffs);

        void toAsn1(asn1::Specie &) const;
        void fromAsn1(const asn1::Specie &);

    private:
        uint64_t coins;
        uint32_t fluffs;
};

}

}
