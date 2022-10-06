#include <fluffycoin/block/Signature.h>

#include <string.h>

using namespace fluffycoin;
using namespace fluffycoin::block;

const BinData &Signature::get() const
{
    return point;
}

void Signature::set(BinData point)
{
    this->point = std::move(point);
}

void Signature::toASN1(ASN1_OCTET_STRING &t) const
{
    ASN1_STRING_set(&t, point.data(), static_cast<int>(point.length()));
}

void Signature::fromASN1(const ASN1_OCTET_STRING &t)
{
    point.setData(ASN1_STRING_get0_data(&t), static_cast<size_t>(ASN1_STRING_length(&t)));
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
