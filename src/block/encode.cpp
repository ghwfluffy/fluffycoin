#include <fluffycoin/block/encode.h>

#include <fluffycoin/ossl/encode.h>

using namespace fluffycoin;

BinData block::encode(const Genesis &t)
{
    asn1::Genesis *obj = asn1::Genesis_new();
    t.toAsn1(*obj);
    BinData ret = ossl::encode(*obj, asn1::i2d_Genesis);
    asn1::Genesis_free(obj);
    return ret;
}
