#pragma once

#include <fluffycoin/block/Exchange.h>

#include <openssl/asn1.h>

#include <string>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Transfer_st Transfer;
    DECLARE_ASN1_FUNCTIONS(Transfer)
}

namespace block
{

/**
 * Send coins from one wallet to another
 */
class Transfer
{
    public:
        Transfer() = default;
        Transfer(Transfer &&) = default;
        Transfer(const Transfer &) = default;
        Transfer &operator=(Transfer &&) = default;
        Transfer &operator=(const Transfer &) = default;
        ~Transfer() = default;

        const std::list<Exchange> &getExchanges() const;
        void setExchanges(std::list<Exchange> exchanges);

        const std::string &getCoin() const;
        void setCoin(std::string coin);

        void toAsn1(asn1::Transfer &) const;
        void fromAsn1(const asn1::Transfer &);

    private:
        std::list<Exchange> exchanges;
        std::string coin;
};

}

}
