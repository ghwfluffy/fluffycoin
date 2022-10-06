#include <fluffycoin/block/PublicKey.h>

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

void PublicKey::toASN1(ASN1_OCTET_STRING &t) const
{
    ASN1_STRING_set(&t, point.data(), static_cast<int>(point.length()));
}

void PublicKey::fromASN1(const ASN1_OCTET_STRING &t)
{
    point.setData(ASN1_STRING_get0_data(&t), static_cast<size_t>(ASN1_STRING_length(&t)));
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
