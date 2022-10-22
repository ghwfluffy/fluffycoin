#pragma once

#include <fluffycoin/block/Specie.h>
#include <fluffycoin/block/Address.h>
#include <fluffycoin/block/PublicKey.h>
#include <fluffycoin/block/Signature.h>

#include <fluffycoin/utils/BinData.h>

#include <openssl/asn1.h>

namespace fluffycoin
{

namespace asn1
{
    typedef struct Register_st Register;
    DECLARE_ASN1_FUNCTIONS(Register)
}

namespace block
{

/**
 * Stake some coin to become a validator
 */
class Register
{
    public:
        Register() = default;
        Register(Register &&) = default;
        Register(const Register &) = default;
        Register &operator=(Register &&) = default;
        Register &operator=(const Register &) = default;
        ~Register() = default;

        const Specie &getStakeAmount() const;
        void setStakeAmount(Specie specie);

        const Address &getStakeAddress() const;
        void setStakeAddress(Address stakeAddress);

        const PublicKey &getStakeKey() const;
        void setStakeKey(PublicKey key);

        const Address &getNewAddress() const;
        void setNewAddress(Address newAddress);

        const Address &getAuthAddress() const;
        void setAuthAddress(Address newAddress);

        const PublicKey &getAuthKey() const;
        void setAuthKey(PublicKey key);

        const Signature &getStakeSignature() const;
        void setStakeSignature(Signature stakeSignature);

        const Signature &getAuthSignature() const;
        void setAuthSignature(Signature authSignature);

        void toAsn1(asn1::Register &) const;
        void fromAsn1(const asn1::Register &);

        BinData toContent() const;

    private:
        Specie stakeAmount;
        Address stakeAddress;
        PublicKey stakeKey;
        Address newAddress;

        Address authAddress;
        PublicKey authKey;

        Signature stakeSignature;
        Signature authSignature;
};

}

}
