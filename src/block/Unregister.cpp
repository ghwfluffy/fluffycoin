#include <fluffycoin/block/Unregister.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Unregister_st
{
    ASN1_OCTET_STRING *authAddress;
    ASN1_OCTET_STRING *authSignature;
} Unregister;

ASN1_SEQUENCE(Unregister)
{
    ASN1_SIMPLE(Unregister, authAddress, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(Unregister, authSignature, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(Unregister)

IMPLEMENT_ASN1_FUNCTIONS(Unregister)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

const Address &Unregister::getAuthAddress() const
{
    return authAddress;
}

void Unregister::setAuthAddress(Address newAddress)
{
    this->authAddress = std::move(newAddress);
}

const Signature &Unregister::getAuthSignature() const
{
    return authSignature;
}

void Unregister::setAuthSignature(Signature authSignature)
{
    this->authSignature = std::move(authSignature);
}

void Unregister::toAsn1(asn1::Unregister &t) const
{
    authAddress.toAsn1(*t.authAddress);
    authSignature.toAsn1(*t.authSignature);
}

void Unregister::fromAsn1(const asn1::Unregister &t)
{
    authAddress.fromAsn1(*t.authAddress);
    authSignature.fromAsn1(*t.authSignature);
}

BinData Unregister::toContent() const
{
    return authAddress.get();
}
