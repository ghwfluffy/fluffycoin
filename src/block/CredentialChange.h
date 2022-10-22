#pragma once

#include <fluffycoin/block/Address.h>
#include <fluffycoin/block/PublicKey.h>
#include <fluffycoin/block/Signature.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct CredentialChange_st CredentialChange;
    DECLARE_ASN1_FUNCTIONS(CredentialChange)
}

namespace block
{

/**
 * Change your authentication key
 */
class CredentialChange
{
    public:
        CredentialChange() = default;
        CredentialChange(CredentialChange &&) = default;
        CredentialChange(const CredentialChange &) = default;
        CredentialChange &operator=(CredentialChange &&) = default;
        CredentialChange &operator=(const CredentialChange &) = default;
        ~CredentialChange() = default;

        const Address &getAuthAddress() const;
        void setAuthAddress(Address newAddress);

        const Address &getNewAuthAddress() const;
        void setNewAuthAddress(Address newAddress);

        const PublicKey &getNewAuthKey() const;
        void setNewAuthKey(PublicKey newAuthKey);

        const Address &getStakeAddress() const;
        void setStakeAddress(Address stakeAddress);

        const PublicKey &getStakeKey() const;
        void setStakeKey(PublicKey stakeKey);

        const Address &getNewStakeAddress() const;
        void setNewStakeAddress(Address newStakeAddress);

        const Signature &getAuthSignature() const;
        void setAuthSignature(Signature authSignature);

        const Signature &getNewAuthSignature() const;
        void setNewAuthSignature(Signature newAuthSignature);

        const Signature &getStakeSignature() const;
        void setStakeSignature(Signature stakeSignature);

        void toAsn1(asn1::CredentialChange &) const;
        void fromAsn1(const asn1::CredentialChange &);

        BinData toContent() const;

    private:
        Address authAddress;
        Address newAuthAddress;
        Address stakeAddress;
        Address newStakeAddress;
        PublicKey newAuthKey;
        PublicKey stakeKey;

        Signature authSignature;
        Signature newAuthSignature;
        Signature stakeSignature;
};

}

}
