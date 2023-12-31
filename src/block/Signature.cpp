#include <fluffycoin/block/Signature.h>
#include <fluffycoin/ossl/convert.h>

#include <string.h>

using namespace fluffycoin;
using namespace fluffycoin::block;

Signature::Signature(BinData sig)
{
    this->point = std::move(sig);
}

const BinData &Signature::get() const
{
    return point;
}

void Signature::set(BinData point)
{
    this->point = std::move(point);
}

void Signature::toAsn1(ASN1_OCTET_STRING &t) const
{
    ossl::fromBin(t, point);
}

void Signature::fromAsn1(const ASN1_OCTET_STRING &t)
{
    point = ossl::toBin(t);
}

bool Signature::operator==(const Signature &rhs) const
{
    return
        rhs.point.length() == point.length() &&
        !memcmp(rhs.point.data(), point.data(), point.length());
}

bool Signature::operator!=(const Signature &rhs) const
{
    return !((*this) == rhs);
}
