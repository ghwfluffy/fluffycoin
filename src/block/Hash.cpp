#include <fluffycoin/block/Hash.h>
#include <fluffycoin/ossl/convert.h>
#include <fluffycoin/ossl/encode.h>
#include <fluffycoin/ossl/Hash.h>

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

Hash::Hash(const BinData &value)
{
    this->sha2 = ossl::Hash::sha2_256(value);
    this->sha3 = ossl::Hash::sha3_256(value);
}

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

void Hash::toAsn1(asn1::Hash &t) const
{
    ossl::fromBin(*t.sha2, sha2);
    ossl::fromBin(*t.sha3, sha3);
}

void Hash::fromAsn1(const asn1::Hash &t)
{
    sha2 = ossl::toBin(*t.sha2);
    sha3 = ossl::toBin(*t.sha3);
}

BinData Hash::encode() const
{
    asn1::Hash *obj = asn1::Hash_new();
    toAsn1(*obj);
    BinData data = ossl::encode(*obj, asn1::i2d_Hash);
    asn1::Hash_free(obj);
    return data;
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
