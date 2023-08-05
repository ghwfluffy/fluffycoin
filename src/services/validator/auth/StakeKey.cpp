#include <fluffycoin/validator/StakeKey.h>

#include <fluffycoin/ossl/Curve25519.h>

using namespace fluffycoin;
using namespace fluffycoin::validator;

const BinData &StakeKey::getPub() const
{
    return pub;
}

const SafeData &StakeKey::getPriv() const
{
    return priv;
}

const ossl::EvpPkeyPtr &StakeKey::getKey() const
{
    return key;
}

const std::string &StakeKey::getAddress() const
{
    return address;
}

void StakeKey::setKey(
    std::string address,
    ossl::EvpPkeyPtr keyPtr)
{
    this->address = std::move(address);
    this->key = std::move(keyPtr);

    if (key)
    {
        pub = ossl::Curve25519::toPublic(*key);
        priv = ossl::Curve25519::toPrivate(*key);
    }
    else
    {
        pub.clear();
        priv.clear();
    }
}
