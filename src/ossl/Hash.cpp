#include <fluffycoin/ossl/Hash.h>

#include <openssl/evp.h>

using namespace fluffycoin;
using namespace fluffycoin::ossl;

namespace
{

BinData doHash(
    const BinData &data,
    const EVP_MD *algorithm)
{
    unsigned int len = EVP_MAX_MD_SIZE;
    unsigned char *digest = reinterpret_cast<unsigned char *>(malloc(len));
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, algorithm, nullptr);
    EVP_DigestUpdate(context, data.data(), data.length());
    EVP_DigestFinal_ex(context, digest, &len);
    EVP_MD_CTX_destroy(context);

    BinData hash;
    hash.setBuffer(digest, static_cast<size_t>(len));

    return hash;
}

}

BinData Hash::sha2_256(const BinData &data)
{
    return doHash(data, EVP_sha256());
}

BinData Hash::sha3_256(const BinData &data)
{
    return doHash(data, EVP_sha3_256());
}
