#include <fluffycoin/block/Register.h>

#include <fluffycoin/ossl/encode.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct RegisterContent_st
{
    Specie *stakeAmount;
    ASN1_OCTET_STRING *stakeAddress;
    ASN1_OCTET_STRING *stakeKey;
    ASN1_OCTET_STRING *newAddress;
    ASN1_OCTET_STRING *authAddress;
    ASN1_OCTET_STRING *authKey;
} RegisterContent;

DECLARE_ASN1_FUNCTIONS(RegisterContent)

ASN1_SEQUENCE(RegisterContent) =
{
    ASN1_SIMPLE(RegisterContent, stakeAmount, Specie)
  , ASN1_SIMPLE(RegisterContent, stakeAddress, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(RegisterContent, stakeKey, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(RegisterContent, newAddress, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(RegisterContent, authAddress, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(RegisterContent, authKey, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(RegisterContent)

IMPLEMENT_ASN1_FUNCTIONS(RegisterContent)

typedef struct Register_st
{
    RegisterContent *content;
    ASN1_OCTET_STRING *stakeSignature;
    ASN1_OCTET_STRING *authSignature;
} Register;

ASN1_SEQUENCE(Register)
{
    ASN1_SIMPLE(Register, content, RegisterContent)
  , ASN1_SIMPLE(Register, stakeSignature, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(Register, authSignature, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(Register)

IMPLEMENT_ASN1_FUNCTIONS(Register)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

const Specie &Register::getStakeAmount() const
{
    return stakeAmount;
}

void Register::setStakeAmount(Specie specie)
{
    this->stakeAmount = std::move(specie);
}

const Address &Register::getStakeAddress() const
{
    return stakeAddress;
}

void Register::setStakeAddress(Address stakeAddress)
{
    this->stakeAddress = std::move(stakeAddress);
}

const PublicKey &Register::getStakeKey() const
{
    return stakeKey;
}

void Register::setStakeKey(PublicKey key)
{
    this->stakeKey = std::move(key);
}

const Address &Register::getNewAddress() const
{
    return newAddress;
}

void Register::setNewAddress(Address newAddress)
{
    this->newAddress = std::move(newAddress);
}

const Address &Register::getAuthAddress() const
{
    return authAddress;
}

void Register::setAuthAddress(Address newAddress)
{
    this->authAddress = std::move(newAddress);
}

const PublicKey &Register::getAuthKey() const
{
    return authKey;
}

void Register::setAuthKey(PublicKey key)
{
    this->authKey = std::move(key);
}

const Signature &Register::getStakeSignature() const
{
    return stakeSignature;
}

void Register::setStakeSignature(Signature stakeSignature)
{
    this->stakeSignature = std::move(stakeSignature);
}

const Signature &Register::getAuthSignature() const
{
    return authSignature;
}

void Register::setAuthSignature(Signature authSignature)
{
    this->authSignature = std::move(authSignature);
}

void Register::toAsn1(asn1::Register &t) const
{
    stakeAmount.toAsn1(*t.content->stakeAmount);
    stakeAddress.toAsn1(*t.content->stakeAddress);
    stakeKey.toAsn1(*t.content->stakeKey);
    newAddress.toAsn1(*t.content->newAddress);
    authAddress.toAsn1(*t.content->authAddress);
    authKey.toAsn1(*t.content->authKey);
    stakeSignature.toAsn1(*t.stakeSignature);
    authSignature.toAsn1(*t.authSignature);
}

void Register::fromAsn1(const asn1::Register &t)
{
    stakeAmount.fromAsn1(*t.content->stakeAmount);
    stakeAddress.fromAsn1(*t.content->stakeAddress);
    stakeKey.fromAsn1(*t.content->stakeKey);
    newAddress.fromAsn1(*t.content->newAddress);
    authAddress.fromAsn1(*t.content->authAddress);
    authKey.fromAsn1(*t.content->authKey);
    stakeSignature.fromAsn1(*t.stakeSignature);
    authSignature.fromAsn1(*t.authSignature);
}

BinData Register::toContent() const
{
    asn1::Register *obj = asn1::Register_new();
    toAsn1(*obj);
    BinData data = ossl::encode(*obj->content, asn1::i2d_RegisterContent);
    asn1::Register_free(obj);
    return data;
}
