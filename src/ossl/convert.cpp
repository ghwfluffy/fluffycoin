#include <fluffycoin/ossl/convert.h>

#include <fluffycoin/log/Log.h>

#include <openssl/objects.h>

using namespace fluffycoin;

BinData ossl::toBin(const ASN1_STRING &t)
{
    return BinData(ASN1_STRING_get0_data(&t), static_cast<size_t>(ASN1_STRING_length(&t)));
}

void ossl::fromBin(ASN1_STRING &t, const BinData &data)
{
    ASN1_STRING_set(&t, data.data(), static_cast<int>(data.length()));
}

std::string ossl::toString(const ASN1_STRING &t)
{
    return std::string(
        reinterpret_cast<const char *>(ASN1_STRING_get0_data(&t)),
        static_cast<size_t>(ASN1_STRING_length(&t)));
}

void ossl::fromString(ASN1_STRING &t, const std::string &str)
{
    ASN1_STRING_set(&t, str.data(), static_cast<int>(str.length()));
}

uint32_t ossl::toUInt32(const ASN1_INTEGER &t)
{
    uint32_t ret = 0;
    BIGNUM *bn = ASN1_INTEGER_to_BN(&t, nullptr);
    if (BN_is_negative(bn) || BN_num_bits(bn) > 32)
    {
        char *value = BN_bn2dec(bn);
        log::error("Invalid ASN.1 uint32 value {}.", value);
        free(value);
    }
    else
    {
        ret = static_cast<uint32_t>(ASN1_INTEGER_get(&t));
    }

    BN_free(bn);
    return ret;
}

void ossl::fromUInt32(ASN1_INTEGER &t, uint32_t ui)
{
    ASN1_INTEGER_set_uint64(&t, static_cast<uint64_t>(ui));
}

uint64_t ossl::toUInt64(const ASN1_INTEGER &t)
{
    uint64_t ret = 0;
    BIGNUM *bn = ASN1_INTEGER_to_BN(&t, nullptr);
    if (BN_is_negative(bn) || BN_num_bits(bn) > 64)
    {
        char *value = BN_bn2dec(bn);
        log::error("Invalid ASN.1 uint64 value {}.", value);
        free(value);
    }
    else
    {
        ret = static_cast<uint64_t>(ASN1_INTEGER_get(&t));
    }

    BN_free(bn);
    return ret;
}

void ossl::fromUInt64(ASN1_INTEGER &t, uint64_t ui)
{
    ASN1_INTEGER_set_uint64(&t, ui);
}

int ossl::toNid(const ASN1_OBJECT &t)
{
    return OBJ_obj2nid(&t);
}

void ossl::fromNid(ASN1_OBJECT *&pt, int nid)
{
    ASN1_OBJECT *lookup = OBJ_nid2obj(nid);
    if (!lookup)
        log::error("Failed to find ASN.1 OID by NID {}.", nid);
    else
    {
        ASN1_OBJECT_free(pt);
        pt = lookup;
    }
}
