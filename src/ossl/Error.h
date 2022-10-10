#pragma once

#include <string>

namespace fluffycoin
{

namespace ossl
{

/**
 * Retrieve OpenSSL errors as loggable strings
 */
namespace Error
{
    std::string pop();
    std::string peek();
    void clear();
}

}

}
