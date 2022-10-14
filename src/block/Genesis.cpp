#include <fluffycoin/block/Genesis.h>

#include <fluffycoin/ossl/convert.h>
#include <fluffycoin/ossl/encode.h>

#include <openssl/asn1t.h>

namespace fluffycoin
{

namespace asn1
{

typedef struct Genesis_st
{
    ASN1_INTEGER *protocol;
    ASN1_UTF8STRING *name;
    ASN1_INTEGER *version;
    ASN1_INTEGER *creation;
    ASN1_OCTET_STRING *creator;
    Specie *greed;
    ASN1_OCTET_STRING *seed;
} Genesis;

ASN1_SEQUENCE(Genesis) =
{
    ASN1_SIMPLE(Genesis, protocol, ASN1_INTEGER)
  , ASN1_SIMPLE(Genesis, name, ASN1_UTF8STRING)
  , ASN1_SIMPLE(Genesis, version, ASN1_INTEGER)
  , ASN1_SIMPLE(Genesis, creation, ASN1_INTEGER)
  , ASN1_SIMPLE(Genesis, creator, ASN1_OCTET_STRING)
  , ASN1_SIMPLE(Genesis, greed, Specie)
  , ASN1_SIMPLE(Genesis, seed, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(Genesis)

IMPLEMENT_ASN1_FUNCTIONS(Genesis)

}

}

using namespace fluffycoin;
using namespace fluffycoin::block;

Genesis::Genesis()
{
    protocol = 0;
    version = 0;
}

uint32_t Genesis::getProtocol() const
{
    return protocol;
}

void Genesis::setProtocol(uint32_t protocol)
{
    this->protocol = protocol;
}

const std::string &Genesis::getName() const
{
    return name;
}

void Genesis::setName(std::string name)
{
    this->name = std::move(name);
}

uint32_t Genesis::getVersion() const
{
    return version;
}

void Genesis::setVersion(uint32_t version)
{
    this->version = version;
}

const Time &Genesis::getCreation() const
{
    return creation;
}

void Genesis::setCreation(Time time)
{
    this->creation = std::move(time);
}

const Address &Genesis::getCreator() const
{
    return creator;
}

void Genesis::setCreator(Address creator)
{
    this->creator = std::move(creator);
}

const Specie &Genesis::getGreed() const
{
    return greed;
}

void Genesis::setGreed(Specie specie)
{
    this->greed = specie;
}

const BinData &Genesis::getSeed() const
{
    return seed;
}

void Genesis::setSeed(BinData data)
{
    this->seed = std::move(data);
}

void Genesis::toAsn1(asn1::Genesis &t) const
{
    ossl::fromUInt64(*t.protocol, protocol);
    ossl::fromBin(*t.name, name);
    ossl::fromUInt64(*t.version, version);
    creation.toAsn1(*t.creation);
    creator.toAsn1(*t.creator);
    greed.toAsn1(*t.greed);
    ossl::fromBin(*t.seed, seed);
}

void Genesis::fromAsn1(const asn1::Genesis &t)
{
    protocol = ossl::toUInt32(*t.protocol);
    name = ossl::toString(*t.name);
    version = ossl::toUInt32(*t.version);
    creation.fromAsn1(*t.creation);
    creator.fromAsn1(*t.creator);
    greed.fromAsn1(*t.greed);
    seed = ossl::toBin(*t.seed);
}

BinData Genesis::toContent() const
{
    asn1::Genesis *obj = asn1::Genesis_new();
    toAsn1(*obj);
    BinData data = ossl::encode(*obj, asn1::i2d_Genesis);
    asn1::Genesis_free(obj);
    return data;
}
