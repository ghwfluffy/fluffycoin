#include <fluffycoin/block/decode.h>

using namespace fluffycoin;
using namespace fluffycoin::block;

bool fluffycoin::block::decode(const BinData &data, Hash &hash)
{
    const unsigned char *p = data.data();
    asn1::Hash *asn1Data = asn1::d2i_Hash(nullptr, &p, static_cast<int>(data.length()));
    if (!asn1Data)
        return false;
    hash.fromAsn1(*asn1Data);
    asn1::Hash_free(asn1Data);
    return true;
}

bool fluffycoin::block::decode(const BinData &data, Block &hash)
{
    const unsigned char *p = data.data();
    asn1::Block *asn1Data = asn1::d2i_Block(nullptr, &p, static_cast<int>(data.length()));
    if (!asn1Data)
        return false;
    hash.fromAsn1(*asn1Data);
    asn1::Block_free(asn1Data);
    return true;
}

bool fluffycoin::block::decode(const BinData &data, Genesis &hash)
{
    const unsigned char *p = data.data();
    asn1::Genesis *asn1Data = asn1::d2i_Genesis(nullptr, &p, static_cast<int>(data.length()));
    if (!asn1Data)
        return false;
    hash.fromAsn1(*asn1Data);
    asn1::Genesis_free(asn1Data);
    return true;
}

bool fluffycoin::block::decode(const BinData &data, Reconciliation &hash)
{
    const unsigned char *p = data.data();
    asn1::Reconciliation *asn1Data = asn1::d2i_Reconciliation(nullptr, &p, static_cast<int>(data.length()));
    if (!asn1Data)
        return false;
    hash.fromAsn1(*asn1Data);
    asn1::Reconciliation_free(asn1Data);
    return true;
}
