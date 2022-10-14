#include <fluffycoin/block/Address.h>
#include <fluffycoin/ossl/convert.h>

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

void Address::toAsn1(ASN1_OCTET_STRING &t) const
{
    ossl::fromBin(t, address);
}

void Address::fromAsn1(const ASN1_OCTET_STRING &t)
{
    address = ossl::toBin(t);
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
