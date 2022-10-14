#include <fluffycoin/block/Validation.h>
#include <fluffycoin/ossl/convert.h>

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

const BinData &Validation::getAddress() const
{
    return address;
}

void Validation::setAddress(BinData address)
{
    this->address = std::move(address);
}

const BinData &Validation::getSignature() const
{
    return signature;
}

void Validation::setSignature(BinData signature)
{
    this->signature = std::move(signature);
}

void Validation::toAsn1(asn1::Validation &t) const
{
    *t.verified = static_cast<int>(verified);
    ossl::fromBin(*t.authAddress, address);
    ossl::fromBin(*t.signature, signature);
}

void Validation::fromAsn1(const asn1::Validation &t)
{
    verified = static_cast<bool>(*t.verified);
    address = ossl::toBin(*t.authAddress);
    signature = ossl::toBin(*t.signature);
}
