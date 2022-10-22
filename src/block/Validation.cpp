#include <fluffycoin/block/Validation.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Validation_st
{
    ASN1_BOOLEAN *verified;
    ASN1_OCTET_STRING *authAddress;
    ASN1_OCTET_STRING *signature;
} Validation;

ASN1_SEQUENCE(Validation) =
{
    ASN1_SIMPLE(Validation, verified, ASN1_BOOLEAN)
  , ASN1_SIMPLE(Validation, authAddress, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(Validation, signature, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(Validation)

IMPLEMENT_ASN1_FUNCTIONS(Validation)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

Validation::Validation()
{
    verified = false;
}

bool Validation::isVerified() const
{
    return verified;
}

void Validation::setVerified(bool verified)
{
    this->verified = verified;
}

const Address &Validation::getAddress() const
{
    return address;
}

void Validation::setAddress(Address address)
{
    this->address = std::move(address);
}

const Signature &Validation::getSignature() const
{
    return signature;
}

void Validation::setSignature(Signature signature)
{
    this->signature = std::move(signature);
}

void Validation::toAsn1(asn1::Validation &t) const
{
    *t.verified = static_cast<int>(verified);
    address.toAsn1(*t.authAddress);
    signature.toAsn1(*t.signature);
}

void Validation::fromAsn1(const asn1::Validation &t)
{
    verified = static_cast<bool>(*t.verified);
    address.fromAsn1(*t.authAddress);
    signature.fromAsn1(*t.signature);
}
