#include "fluffytest.h"

#include <fluffycoin/ossl/Initialize.h>
#include <fluffycoin/log/Log.h>

using namespace fluffycoin;

int main(int argc, const char *argv[])
{
    // Initialize google tests
    ::testing::InitGoogleTest(&argc, const_cast<char **>(argv));

    // Initialize fluffycoin
    log::setConsole(true);
    log::setLevel(log::Level::Traffic);
    ossl::Initialize::initialize();

    // Run tests
    int ret = RUN_ALL_TESTS();

    // Cleanup
    ossl::Initialize::cleanup();

    return ret;
}