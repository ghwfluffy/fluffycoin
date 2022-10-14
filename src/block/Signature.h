#pragma once

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace block
{

/**
 * 32-byte x coordinate | 32-byte y coordinate
 */
class Signature
{
    public:
        Signature() = default;
        Signature(Signature &&) = default;
        Signature(const Signature &) = default;
        Signature &operator=(Signature &&) = default;
        Signature &operator=(const Signature &) = default;
        ~Signature() = default;

        const BinData &get() const;
        void set(BinData point);

        void toAsn1(ASN1_OCTET_STRING &t) const;
        void fromAsn1(const ASN1_OCTET_STRING &t);

        bool operator==(const Signature &) const;
        bool operator!=(const Signature &) const;

    private:
        BinData point;
};

}

}
