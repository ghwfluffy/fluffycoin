#include <fluffycoin/block/Transfer.h>
#include <fluffycoin/ossl/convert.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Transfer_st
{
    STACK_OF(Exchange) *exchanges;
    ASN1_UTF8STRING *coin;
} Transfer;

ASN1_SEQUENCE(Transfer) =
{
    ASN1_SEQUENCE_OF(Transfer, exchanges, Exchange)
  , ASN1_SIMPLE(Transfer, coin, ASN1_UTF8STRING)
} ASN1_SEQUENCE_END(Transfer)

IMPLEMENT_ASN1_FUNCTIONS(Transfer)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

const std::list<Exchange> &Transfer::getExchanges() const
{
    return exchanges;
}

void Transfer::setExchanges(std::list<Exchange> exchanges)
{
    this->exchanges = std::move(exchanges);
}

const std::string &Transfer::getCoin() const
{
    return coin;
}

void Transfer::setCoin(std::string coin)
{
    this->coin = std::move(coin);
}

void Transfer::toAsn1(asn1::Transfer &t) const
{
    asn1::toExchangeStack(*t.exchanges, exchanges);
    ossl::fromString(*t.coin, coin);
}

void Transfer::fromAsn1(const asn1::Transfer &t)
{
    asn1::fromExchangeStack(exchanges, *t.exchanges);
    coin = ossl::toString(*t.coin);
}
