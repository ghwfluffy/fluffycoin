#include <fluffycoin/alg/Address.h>

#include <fluffycoin/ossl/Curve25519.h>
#include <fluffycoin/ossl/Prng.h>
#include <fluffycoin/ossl/Hash.h>

#include <fluffycoin/utils/Hex.h>
#include <fluffycoin/utils/Base64.h>

#include <fluffycoin/log/Log.h>

using namespace fluffycoin;
using namespace fluffycoin::alg;

BinData Address::generate(const EVP_PKEY &key)
{
    return generate(key, BinData());
}

BinData Address::generate(const EVP_PKEY &key, const char *seed)
{
    return generate(key, std::string(seed));
}

BinData Address::generate(const EVP_PKEY &key, const std::string &printableSeed)
{
    // Need to make it valid Base64 data
    std::string paddedSeed(printableSeed);
    if (paddedSeed.length() & 3)
    {
        size_t padLen = 4 - (paddedSeed.length() & 3);
        std::string rand;
        for (size_t ui = 0; ui < padLen; ui++)
            rand += Base64::valToChar(static_cast<unsigned char>(ossl::Prng::randInt(0, 63)));
        log::traffic("Generated {} chars for printable address seed '{}'.", padLen, rand);
        paddedSeed += rand;
    }

    return generate(key, Base64::decode(paddedSeed));
}

BinData Address::generate(const EVP_PKEY &key, const BinData &seed)
{
    // Seed needs to be specific length
    // Can be user chosen or randomly generated
    BinData paddedSeed(seed);

    // Enforce seed length
    if (paddedSeed.length() > Address::SEED_LENGTH)
    {
        log::error("Invalid address seed length {}.", paddedSeed.length());
        paddedSeed.resize(Address::SEED_LENGTH);
    }
    else if (paddedSeed.length() < Address::SEED_LENGTH)
    {
        size_t randBytes = Address::SEED_LENGTH - paddedSeed.length();
        BinData rand = ossl::Prng::rand(randBytes);
        log::traffic("Generated {} bytes for address seed '{}'.", randBytes, Hex::encode(rand));
        paddedSeed.append(rand.data(), rand.length());
    }

    // Serialize public key
    BinData publicKey = ossl::Curve25519::toPublic(key);

    // Hash public key with seed
    BinData input;
    // SHA3-256(Seed | Public Key)
    input.append(paddedSeed.data(), paddedSeed.length());
    input.append(publicKey.data(), publicKey.length());
    BinData sha3 = ossl::Hash::sha3_256(input);

    // SHA2-256(Seed | SHA3-256)
    input = BinData();
    input.append(paddedSeed.data(), paddedSeed.length());
    input.append(sha3.data(), sha3.length());
    BinData sha2 = ossl::Hash::sha2_256(input);

    // Address is [Seed | trunc(Hash, 20)]
    constexpr const size_t TRUNCATED_HASH_LEN = 20;
    BinData address;
    address.append(paddedSeed.data(), paddedSeed.length());
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

    BinData seed(address.data(), Address::SEED_LENGTH);
    BinData newAddress = generate(key, seed);
    return address == newAddress;
}

std::string Address::printable(const BinData &address)
{
    return Base64::encode(address);
}

BinData Address::unprintable(const std::string &address)
{
    return Base64::decode(address);
}
