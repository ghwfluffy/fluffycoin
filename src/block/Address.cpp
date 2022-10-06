#include <fluffycoin/block/Address.h>

#include <stdlib.h>
#include <string.h>

using namespace fluffycoin;
using namespace fluffycoin::block;

const BinData &Address::get() const
{
    return address;
}

void Address::set(BinData address)
{
    this->address = std::move(address);
}

void Address::toASN1(ASN1_OCTET_STRING &t) const
{
    ASN1_STRING_set(&t, address.data(), static_cast<int>(address.length()));
}

void Address::fromASN1(const ASN1_OCTET_STRING &t)
{
    address.setData(ASN1_STRING_get0_data(&t), static_cast<size_t>(ASN1_STRING_length(&t)));
}

bool Address::operator==(const Address &rhs) const
{
    return
        rhs.address.length() == address.length() &&
        !memcmp(rhs.address.data(), address.data(), address.length());
}

bool Address::operator!=(const Address &rhs) const
{
    return !((*this) == rhs);
}
