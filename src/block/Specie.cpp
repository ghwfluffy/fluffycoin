#include <fluffycoin/block/Specie.h>
#include <fluffycoin/ossl/convert.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Specie_st
{
    ASN1_INTEGER *coins;
    ASN1_INTEGER *fluffs;
} Specie;

ASN1_SEQUENCE(Specie) = {
    ASN1_SIMPLE(Specie, coins, ASN1_INTEGER)
  , ASN1_SIMPLE(Specie, fluffs, ASN1_INTEGER)
} ASN1_SEQUENCE_END(Specie)

IMPLEMENT_ASN1_FUNCTIONS(Specie)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

Specie::Specie()
{
    coins = 0;
    fluffs = 0;
}

uint64_t Specie::getCoins() const
{
    return coins;
}

void Specie::setCoins(uint64_t coins)
{
    this->coins = coins;
}

uint32_t Specie::getFluffs() const
{
    return fluffs;
}

void Specie::setFluffs(uint32_t fluffs)
{
    this->fluffs = fluffs;
}

void Specie::toASN1(asn1::Specie &t) const
{
    ossl::fromUInt64(*t.coins, coins);
    ossl::fromUInt32(*t.fluffs, fluffs);
}

void Specie::fromASN1(const asn1::Specie &t)
{
    coins = ossl::toUInt64(*t.coins);
    fluffs = ossl::toUInt32(*t.fluffs);
}
