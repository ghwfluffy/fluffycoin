#include <fluffycoin/block/Exchange.h>
#include <fluffycoin/ossl/convert.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Exchange_st
{
    ASN1_OCTET_STRING *receiver;
    Specie *amount;
} Exchange;

ASN1_SEQUENCE(Exchange) =
{
    ASN1_SIMPLE(Exchange, receiver, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(Exchange, amount, Specie)
} ASN1_SEQUENCE_END(Exchange)

IMPLEMENT_ASN1_FUNCTIONS(Exchange)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

const Address &Exchange::getReceiver() const
{
    return receiver;
}

void Exchange::setReceiver(Address receiver)
{
    this->receiver = std::move(receiver);
}

const Specie &Exchange::getAmount() const
{
    return amount;
}

void Exchange::setAmount(Specie amount)
{
    this->amount = std::move(amount);
}

void Exchange::toAsn1(asn1::Exchange &t) const
{
    receiver.toAsn1(*t.receiver);
    amount.toAsn1(*t.amount);
}

void Exchange::fromAsn1(const asn1::Exchange &t)
{
    receiver.fromAsn1(*t.receiver);
    amount.fromAsn1(*t.amount);
}
