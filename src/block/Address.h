#pragma once

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace block
{

/**
 * SHA2-256(SHA3-256(Curve25519 x coordinate))
 */
class Address
{
    public:
        Address() = default;
        Address(Address &&) = default;
        Address(const Address &) = default;
        Address &operator=(Address &&) = default;
        Address &operator=(const Address &) = default;
        ~Address() = default;

        const BinData &get() const;
        void set(BinData address);

        void toASN1(ASN1_OCTET_STRING &t) const;
        void fromASN1(const ASN1_OCTET_STRING &t);

        bool operator==(const Address &) const;
        bool operator!=(const Address &) const;

    private:
        BinData address;
};

}

}
