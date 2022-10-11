#pragma once

#include <fluffycoin/block/Specie.h>

#include <fluffycoin/ossl/convert.h>

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Exchange_st Exchange;
    DECLARE_ASN1_FUNCTIONS(Exchange)
    FLUFFYCOIN_DEFINE_STACK_FUNCTIONS(Exchange)
}

namespace block
{

/**
 * Amount of coins to transfer and receiver
 */
class Exchange
{
    public:
        Exchange() = default;
        Exchange(Exchange &&) = default;
        Exchange(const Exchange &) = default;
        Exchange &operator=(Exchange &&) = default;
        Exchange &operator=(const Exchange &) = default;
        ~Exchange() = default;

        const BinData &getReceiver() const;
        void setReceiver(BinData receiver);

        const Specie &getAmount() const;
        void setAmount(Specie amount);

        void toASN1(asn1::Exchange &) const;
        void fromASN1(const asn1::Exchange &);

    private:
        BinData receiver;
        Specie amount;
};

}

}
