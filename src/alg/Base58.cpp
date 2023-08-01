#include <fluffycoin/alg/Base58.h>

#include <fluffycoin/log/Log.h>

#include <openssl/bn.h>

using namespace fluffycoin;
using namespace fluffycoin::alg;

unsigned char Base58::charToVal(char curChar)
{
    unsigned char ret = 0;
    if (curChar >= '1' && curChar <= '9')
        ret = static_cast<unsigned char>(curChar - '1');
    else if (curChar >= 'A' && curChar <= 'H')
        ret = static_cast<unsigned char>(curChar - 'A' + 9);
    else if (curChar >= 'J' && curChar <= 'N')
        ret = static_cast<unsigned char>(curChar - 'J' + 17);
    else if (curChar >= 'P' && curChar <= 'Z')
        ret = static_cast<unsigned char>(curChar - 'P' + 22);
    else if (curChar >= 'a' && curChar <= 'k')
        ret = static_cast<unsigned char>(curChar - 'a' + 33);
    else if (curChar >= 'm' && curChar <= 'z')
        ret = static_cast<unsigned char>(curChar - 'm' + 44);
    else
        log::error("Invalid Base58 character {}.", curChar);
    return ret;
}

namespace
{

constexpr const char b58Encoding[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
static_assert((sizeof(b58Encoding) - sizeof('\0')) == 58, "base58 encoding requires 58 characters.");

}

char Base58::valToChar(unsigned char val)
{
    char ret = '\0';
    if (val >= sizeof(b58Encoding))
        log::error("Invalid Base58 value {}.", val);
    else
        ret = b58Encoding[val];
    return ret;
}

std::string Base58::encode(const BinData &binary)
{
    if (binary.empty())
        return std::string();

    // Skip zero bytes
    size_t zeros = 0;
    while (zeros < binary.length() && binary.data()[zeros] == 0)
        zeros++;

    // log(256) / log(58), rounded up, plus null terminator
    size_t encodedSize = binary.length() * 138 / 100 + 2;
    char *encoded = static_cast<char *>(calloc(1, encodedSize));

    // Convert value to BIGNUM
    BIGNUM *bn = BN_bin2bn(binary.data(), static_cast<int>(binary.length()), nullptr);

    // Setup utilities we will use
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *rem = BN_new();
    BIGNUM *bn58 = BN_new();
    BN_set_word(bn58, 58);

    // Convert from BIGNUM to string
    size_t pos = 0;
    while (!BN_is_zero(bn))
    {
        BN_div(bn, rem, bn, bn58, ctx);
        unsigned char val = static_cast<unsigned char>(BN_get_word(rem));
        encoded[pos++] = valToChar(val);
    }

    // Prepend zeros and convert to C++ string
    std::string ret;
    ret.resize(pos + zeros, '1');
    for (size_t ui = 0; ui < pos; ui++)
        ret[ret.length() - ui - 1] = encoded[ui];

    // Cleanup
    BN_free(bn);
    BN_free(bn58);
    BN_free(rem);
    BN_CTX_free(ctx);
    free(encoded);

    return ret;
}

BinData Base58::decode(const std::string &printable)
{
    // How many zeros will be prepend
    size_t zeros = 0;
    while (zeros < printable.length() && printable[zeros] == '1')
        zeros++;

    // Convert to BIGNUM
    BIGNUM *bn = BN_new();
    BN_set_word(bn, 0);
    for (size_t ui = zeros; ui < printable.length(); ui++)
    {
        unsigned int val = charToVal(printable[ui]);
        BN_mul_word(bn, 58);
        BN_add_word(bn, val);
    }

    // Convert to binary
    unsigned char *bin = static_cast<unsigned char *>(calloc(1, printable.length()));
    int binlen = BN_bn2bin(bn, bin + zeros);

    // Wrap in BinData
    BinData ret;
    ret.setBuffer(bin, static_cast<size_t>(binlen) + zeros);

    // Cleanup
    BN_free(bn);

    return ret;
}
