#pragma once

#include <fluffycoin/utils/BinData.h>
#include <fluffycoin/ossl/convert.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Validation_st Validation;
    DECLARE_ASN1_FUNCTIONS(Validation)
    FLUFFYCOIN_DEFINE_STACK_FUNCTIONS(Validation)
}

namespace block
{

/**
 * Validator voting on if a block should be accepted
 */
class Validation
{
    public:
        Validation();
        Validation(Validation &&) = default;
        Validation(const Validation &) = default;
        Validation &operator=(Validation &&) = default;
        Validation &operator=(const Validation &) = default;
        ~Validation() = default;

        bool isVerified() const;
        void setVerified(bool verified);

        const BinData &getAddress() const;
        void setAddress(BinData address);

        const BinData &getSignature() const;
        void setSignature(BinData signature);

        void toAsn1(asn1::Validation &) const;
        void fromAsn1(const asn1::Validation &);

    private:
        bool verified;
        BinData address;
        BinData signature;
};

}

}
