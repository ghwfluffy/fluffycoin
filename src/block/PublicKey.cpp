#include <fluffycoin/block/PublicKey.h>
#include <fluffycoin/ossl/convert.h>

#include <string.h>

using namespace fluffycoin;
using namespace fluffycoin::block;

const BinData &PublicKey::get() const
{
    return point;
}

void PublicKey::set(BinData point)
{
    this->point = std::move(point);
}

void PublicKey::toAsn1(ASN1_OCTET_STRING &t) const
{
    ossl::fromBin(t, point);
}

void PublicKey::fromAsn1(const ASN1_OCTET_STRING &t)
{
    point = ossl::toBin(t);
}

bool PublicKey::operator==(const PublicKey &rhs) const
{
    return
        rhs.point.length() == point.length() &&
        !memcmp(rhs.point.data(), point.data(), point.length());
}

bool PublicKey::operator!=(const PublicKey &rhs) const
{
    return !((*this) == rhs);
}
