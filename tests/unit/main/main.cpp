#include "fluffytest.h"

#include <fluffycoin/ossl/Initialize.h>
#include <fluffycoin/log/Log.h>
#include <fluffycoin/svc/Log.h>

using namespace fluffycoin;

int main(int argc, const char *argv[])
{
    // Initialize google tests
    ::testing::InitGoogleTest(&argc, const_cast<char **>(argv));

    // Initialize fluffycoin
    log::setConsole(true);
    log::setLevel(log::Level::Traffic);
    log::Category::init();
    ossl::Initialize::initialize();

    svc::initLogger();

    // Run tests
    int ret = RUN_ALL_TESTS();

    // Cleanup
    ossl::Initialize::cleanup();

    return ret;
}
