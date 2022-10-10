#include <fluffycoin/ossl/Error.h>

#include <openssl/err.h>

using namespace fluffycoin;

namespace
{

std::string errString(unsigned long e)
{
    constexpr const size_t ERROR_LEN_MAX = 512;
    char szError[ERROR_LEN_MAX] = {};
    ERR_error_string_n(e, szError, sizeof(szError));
    return szError;
}

}

std::string ossl::Error::pop()
{
    std::string ret = peek();
    clear();
    return ret;
}

std::string ossl::Error::peek()
{
    return errString(ERR_peek_error());
}

void ossl::Error::clear()
{
    ERR_clear_error();
}
