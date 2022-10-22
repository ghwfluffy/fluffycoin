#include <fluffycoin/block/CredentialChange.h>

#include <fluffycoin/ossl/encode.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct CredentialChangeContent_st
{
    ASN1_OCTET_STRING *authAddress;
    ASN1_OCTET_STRING *newAuthAddress;
    ASN1_OCTET_STRING *newAuthKey;
    ASN1_OCTET_STRING *stakeAddress;
    ASN1_OCTET_STRING *stakeKey;
    ASN1_OCTET_STRING *newStakeAddress;
} CredentialChangeContent;

DECLARE_ASN1_FUNCTIONS(CredentialChangeContent)

ASN1_SEQUENCE(CredentialChangeContent) =
{
    ASN1_SIMPLE(CredentialChangeContent, authAddress, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(CredentialChangeContent, newAuthAddress, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(CredentialChangeContent, newAuthKey, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(CredentialChangeContent, stakeAddress, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(CredentialChangeContent, stakeKey, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(CredentialChangeContent, newStakeAddress, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(CredentialChangeContent)

IMPLEMENT_ASN1_FUNCTIONS(CredentialChangeContent)

typedef struct CredentialChange_st
{
    CredentialChangeContent *content;
    ASN1_OCTET_STRING *authSignature;
    ASN1_OCTET_STRING *newAuthSignature;
    ASN1_OCTET_STRING *stakeSignature;
} CredentialChange;

ASN1_SEQUENCE(CredentialChange)
{
    ASN1_SIMPLE(CredentialChange, content, CredentialChangeContent)
  , ASN1_SIMPLE(CredentialChange, authSignature, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(CredentialChange, newAuthSignature, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(CredentialChange, stakeSignature, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(CredentialChange)

IMPLEMENT_ASN1_FUNCTIONS(CredentialChange)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

const Address &CredentialChange::getAuthAddress() const
{
    return authAddress;
}

void CredentialChange::setAuthAddress(Address authAddress)
{
    this->authAddress = std::move(authAddress);
}

const Address &CredentialChange::getNewAuthAddress() const
{
    return newAuthAddress;
}

void CredentialChange::setNewAuthAddress(Address newAuthAddress)
{
    this->newAuthAddress = std::move(newAuthAddress);
}

const PublicKey &CredentialChange::getNewAuthKey() const
{
    return newAuthKey;
}

void CredentialChange::setNewAuthKey(PublicKey key)
{
    this->newAuthKey = std::move(key);
}

const Address &CredentialChange::getStakeAddress() const
{
    return stakeAddress;
}

void CredentialChange::setStakeAddress(Address stakeAddress)
{
    this->stakeAddress = std::move(stakeAddress);
}

const PublicKey &CredentialChange::getStakeKey() const
{
    return stakeKey;
}

void CredentialChange::setStakeKey(PublicKey stakeKey)
{
    this->stakeKey = std::move(stakeKey);
}

const Address &CredentialChange::getNewStakeAddress() const
{
    return newStakeAddress;
}

void CredentialChange::setNewStakeAddress(Address newStakeAddress)
{
    this->newStakeAddress = std::move(newStakeAddress);
}

const Signature &CredentialChange::getAuthSignature() const
{
    return authSignature;
}

void CredentialChange::setAuthSignature(Signature authSignature)
{
    this->authSignature = std::move(authSignature);
}

const Signature &CredentialChange::getNewAuthSignature() const
{
    return newAuthSignature;
}

void CredentialChange::setNewAuthSignature(Signature newAuthSignature)
{
    this->newAuthSignature = std::move(newAuthSignature);
}

const Signature &CredentialChange::getStakeSignature() const
{
    return stakeSignature;
}

void CredentialChange::setStakeSignature(Signature stakeSignature)
{
    this->stakeSignature = std::move(stakeSignature);
}

void CredentialChange::toAsn1(asn1::CredentialChange &t) const
{
    authAddress.toAsn1(*t.content->authAddress);
    newAuthAddress.toAsn1(*t.content->newAuthAddress);
    stakeAddress.toAsn1(*t.content->stakeAddress);
    newStakeAddress.toAsn1(*t.content->newStakeAddress);
    newAuthKey.toAsn1(*t.content->newAuthKey);
    stakeKey.toAsn1(*t.content->stakeKey);
    authSignature.toAsn1(*t.authSignature);
    newAuthSignature.toAsn1(*t.newAuthSignature);
    stakeSignature.toAsn1(*t.stakeSignature);
}

void CredentialChange::fromAsn1(const asn1::CredentialChange &t)
{
    authAddress.fromAsn1(*t.content->authAddress);
    newAuthAddress.fromAsn1(*t.content->newAuthAddress);
    stakeAddress.fromAsn1(*t.content->stakeAddress);
    newStakeAddress.fromAsn1(*t.content->newStakeAddress);
    newAuthKey.fromAsn1(*t.content->newAuthKey);
    stakeKey.fromAsn1(*t.content->stakeKey);
    authSignature.fromAsn1(*t.authSignature);
    newAuthSignature.fromAsn1(*t.newAuthSignature);
    stakeSignature.fromAsn1(*t.stakeSignature);
}

BinData CredentialChange::toContent() const
{
    asn1::CredentialChange *obj = asn1::CredentialChange_new();
    toAsn1(*obj);
    BinData data = ossl::encode(*obj->content, asn1::i2d_CredentialChangeContent);
    asn1::CredentialChange_free(obj);
    return data;
}
