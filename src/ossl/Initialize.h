#pragma once

namespace fluffycoin
{

namespace ossl
{

/**
 * Initialize the OpenSSL library
 */
namespace Initialize
{
    void initialize();
    void cleanup();
}

}

}
