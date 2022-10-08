#pragma once

#include <fluffycoin/block/Specie.h>

#include <fluffycoin/utils/BinData.h>

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

        const BinData &getReceiver() const;
        void setReceiver(BinData receiver);

        const Specie &getAmount() const;
        void setAmount(Specie amount);

        const std::string &getCoin() const;
        void setCoin(std::string coin);

        void toASN1(asn1::Transfer &) const;
        void fromASN1(const asn1::Transfer &);

    private:
        BinData receiver;
        Specie amount;
        std::string coin;
};

}

}
