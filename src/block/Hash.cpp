#include <fluffycoin/block/Hash.h>

#include <openssl/asn1t.h>

#include <string.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Hash_st
{
    ASN1_OCTET_STRING *sha2;
    ASN1_OCTET_STRING *sha3;
} Hash;

ASN1_SEQUENCE(Hash) = {
    ASN1_SIMPLE(Hash, sha2, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(Hash, sha3, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(Hash)

IMPLEMENT_ASN1_FUNCTIONS(Hash)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

const BinData &Hash::getSha2() const
{
    return sha2;
}

void Hash::setSha2(BinData hash)
{
    this->sha2 = std::move(hash);
}

const BinData &Hash::getSha3() const
{
    return sha3;
}

void Hash::setSha3(BinData hash)
{
    this->sha3 = std::move(hash);
}

void Hash::toASN1(asn1::Hash &t) const
{
    ASN1_STRING_set(t.sha2, sha2.data(), static_cast<int>(sha2.length()));
    ASN1_STRING_set(t.sha3, sha3.data(), static_cast<int>(sha3.length()));
}

void Hash::fromASN1(const asn1::Hash &t)
{
    sha2.setData(ASN1_STRING_get0_data(t.sha2), static_cast<size_t>(ASN1_STRING_length(t.sha2)));
    sha3.setData(ASN1_STRING_get0_data(t.sha3), static_cast<size_t>(ASN1_STRING_length(t.sha3)));
}

bool Hash::operator==(const Hash &rhs) const
{
    return
        sha2.length() == rhs.sha2.length() &&
        sha3.length() == rhs.sha3.length() &&
        !memcmp(sha2.data(), rhs.sha2.data(), sha2.length()) &&
        !memcmp(sha3.data(), rhs.sha3.data(), sha3.length());
}

bool Hash::operator!=(const Hash &rhs) const
{
    return !((*this) == rhs);
}
