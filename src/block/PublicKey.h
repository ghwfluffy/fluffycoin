#pragma once

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace block
{

/**
 * Curve25519 x coordinate
 */
class PublicKey
{
    public:
        PublicKey() = default;
        PublicKey(PublicKey &&) = default;
        PublicKey(const PublicKey &) = default;
        PublicKey &operator=(PublicKey &&) = default;
        PublicKey &operator=(const PublicKey &) = default;
        ~PublicKey() = default;

        const BinData &get() const;
        void set(BinData point);

        void toAsn1(ASN1_OCTET_STRING &t) const;
        void fromAsn1(const ASN1_OCTET_STRING &t);

        bool operator==(const PublicKey &) const;
        bool operator!=(const PublicKey &) const;

    private:
        BinData point;
};

}

}
