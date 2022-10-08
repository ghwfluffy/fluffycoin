#include <fluffycoin/block/Transfer.h>
#include <fluffycoin/ossl/convert.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Transfer_st
{
    ASN1_OCTET_STRING *receiver;
    Specie *amount;
    ASN1_UTF8STRING *coin;
} Transfer;

ASN1_SEQUENCE(Transfer) =
{
    ASN1_SIMPLE(Transfer, receiver, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(Transfer, amount, Specie)
  , ASN1_SIMPLE(Transfer, coin, ASN1_UTF8STRING)
} ASN1_SEQUENCE_END(Transfer)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

const BinData &Transfer::getReceiver() const
{
    return receiver;
}

void Transfer::setReceiver(BinData receiver)
{
    this->receiver = std::move(receiver);
}

const Specie &Transfer::getAmount() const
{
    return amount;
}

void Transfer::setAmount(Specie amount)
{
    this->amount = std::move(amount);
}

const std::string &Transfer::getCoin() const
{
    return coin;
}

void Transfer::setCoin(std::string coin)
{
    this->coin = std::move(coin);
}

void Transfer::toASN1(asn1::Transfer &t) const
{
    ossl::fromBin(*t.receiver, receiver);
    amount.toASN1(*t.amount);
    ossl::fromString(*t.coin, coin);
}

void Transfer::fromASN1(const asn1::Transfer &t)
{
    receiver = ossl::toBin(*t.receiver);
    amount.fromASN1(*t.amount);
    coin = ossl::toString(*t.coin);
}
