#pragma once

#include <fluffycoin/block/Address.h>
#include <fluffycoin/block/Signature.h>

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Unregister_st Unregister;
    DECLARE_ASN1_FUNCTIONS(Unregister)
}

namespace block
{

/**
 * Unstake your coins and stop being a validator
 */
class Unregister
{
    public:
        Unregister() = default;
        Unregister(Unregister &&) = default;
        Unregister(const Unregister &) = default;
        Unregister &operator=(Unregister &&) = default;
        Unregister &operator=(const Unregister &) = default;
        ~Unregister() = default;

        const Address &getAuthAddress() const;
        void setAuthAddress(Address newAddress);

        const Signature &getAuthSignature() const;
        void setAuthSignature(Signature authSignature);

        void toAsn1(asn1::Unregister &) const;
        void fromAsn1(const asn1::Unregister &);

        BinData toContent() const;

    private:
        Address authAddress;
        Signature authSignature;
};

}

}
