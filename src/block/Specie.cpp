#include <fluffycoin/block/Specie.h>
#include <fluffycoin/ossl/convert.h>
#include <fluffycoin/log/Log.h>

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
    if (fluffs >= 100000000)
        log::error(log::General, "Invalid fluff amount {}.", fluffs);
}

void Specie::toAsn1(asn1::Specie &t) const
{
    ossl::fromUInt64(*t.coins, coins);
    ossl::fromUInt32(*t.fluffs, fluffs);
}

void Specie::fromAsn1(const asn1::Specie &t)
{
    coins = ossl::toUInt64(*t.coins);
    fluffs = ossl::toUInt32(*t.fluffs);
}

std::string fluffycoin::to_string(const block::Specie &specie)
{
    char szRet[64] = {};
    int pos = snprintf(szRet, sizeof(szRet),
        "%llu.%08u",
        static_cast<unsigned long long>(specie.getCoins()),
        static_cast<unsigned int>(specie.getFluffs()));

    // Remove zeros from decimal place
    while (pos > 1 && szRet[pos - 1] == '0')
        szRet[--pos] = '\0';

    // Remove decimal if no fluffs
    if (szRet[pos - 1] == '.')
        szRet[--pos] = '\0';

    return std::string(szRet, static_cast<size_t>(pos));
}
