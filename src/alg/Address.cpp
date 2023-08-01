#include <fluffycoin/alg/Address.h>
#include <fluffycoin/alg/Base58.h>

#include <fluffycoin/ossl/Curve25519.h>
#include <fluffycoin/ossl/Prng.h>
#include <fluffycoin/ossl/Hash.h>

#include <fluffycoin/utils/Hex.h>
#include <fluffycoin/utils/Base64.h>

#include <fluffycoin/log/Log.h>

using namespace fluffycoin;
using namespace fluffycoin::alg;

namespace
{

constexpr const size_t ENCODED_SALT_LENGTH = 8;

}

BinData Address::generate(const EVP_PKEY &key)
{
    // If we're generating the salt, it will be valid base58 characters
    std::string salt;
    salt.reserve(ENCODED_SALT_LENGTH);
    for (size_t ui = 0; ui < ENCODED_SALT_LENGTH; ui++)
        salt += Base58::valToChar(static_cast<unsigned char>(ossl::Prng::randInt(0, 57)));
    //log::traffic("Generated printable address salt '{}'.", salt);
    return generate(key, salt);
}

BinData Address::generate(const EVP_PKEY &key, const char *salt)
{
    return generate(key, std::string(salt));
}

BinData Address::generate(const EVP_PKEY &key, const std::string &printableSalt)
{
    // Need to make it valid Base64 data
    std::string paddedSalt(printableSalt);
    if (paddedSalt.length() & 3)
    {
        size_t padLen = 4 - (paddedSalt.length() & 3);
        std::string rand;
        for (size_t ui = 0; ui < padLen; ui++)
            rand += Base58::valToChar(static_cast<unsigned char>(ossl::Prng::randInt(0, 57)));
        //log::traffic("Generated {} chars for printable address salt '{}'.", padLen, rand);
        paddedSalt += rand;
    }

    return generate(key, Base64::decode(paddedSalt));
}

BinData Address::generate(const EVP_PKEY &key, const BinData &salt)
{
    // Salt needs to be specific length
    // Can be user chosen or randomly generated
    BinData paddedSalt(salt);

    // Enforce salt length
    if (paddedSalt.length() > Address::SALT_LENGTH)
    {
        log::error("Invalid address salt length {}.", paddedSalt.length());
        paddedSalt.resize(Address::SALT_LENGTH);
    }
    else if (paddedSalt.length() < Address::SALT_LENGTH)
    {
        size_t randBytes = Address::SALT_LENGTH - paddedSalt.length();
        BinData rand = ossl::Prng::rand(randBytes);
        //log::traffic("Generated {} bytes for address salt '{}'.", randBytes, Hex::encode(rand));
        paddedSalt.append(rand.data(), rand.length());
    }

    // Serialize public key
    BinData publicKey = ossl::Curve25519::toPublic(key);

    // Hash public key with salt
    BinData input;
    // SHA3-256(Salt | Public Key)
    input.append(paddedSalt.data(), paddedSalt.length());
    input.append(publicKey.data(), publicKey.length());
    BinData sha3 = ossl::Hash::sha3_256(input);

    // SHA2-256(Salt | SHA3-256)
    input = BinData();
    input.append(paddedSalt.data(), paddedSalt.length());
    input.append(sha3.data(), sha3.length());
    BinData sha2 = ossl::Hash::sha2_256(input);

    // Address is [Salt | trunc(Hash, 20)]
    constexpr const size_t TRUNCATED_HASH_LEN = 20;
    BinData address;
    address.append(paddedSalt.data(), paddedSalt.length());
    address.append(sha2.data(), TRUNCATED_HASH_LEN);

    return address;
}

bool Address::verify(const BinData &address, const EVP_PKEY &key)
{
    if (address.length() != Address::LENGTH)
    {
        log::error("Invalid verify address length {}.", address.length());
        return false;
    }

    BinData salt(address.data(), Address::SALT_LENGTH);
    BinData newAddress = generate(key, salt);
    return address == newAddress;
}

bool Address::verify(const std::string &address, const EVP_PKEY &key)
{
    return verify(unprintable(address), key);
}

std::string Address::printable(const BinData &address)
{
    std::string ret;
    ret = Base64::encode(address.sub(0, std::min(SALT_LENGTH, address.length())));
    if (address.length() > SALT_LENGTH)
        ret += Base58::encode(address.sub(SALT_LENGTH, address.length() - SALT_LENGTH));

    return ret;
}

BinData Address::unprintable(const std::string &address)
{
    BinData ret;
    ret = Base64::decode(address.substr(0, std::min(ENCODED_SALT_LENGTH, address.length())));
    if (address.length() > ENCODED_SALT_LENGTH)
        ret += Base58::decode(address.substr(ENCODED_SALT_LENGTH));

    return ret;
}
