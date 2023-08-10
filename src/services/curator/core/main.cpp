#include <fluffycoin/curator/CuratorServiceParams.h>

#include <fluffycoin/svc/Run.h>

using namespace fluffycoin;
using namespace fluffycoin::curator;

int main(int argc, const char *argv[])
{
    return svc::run<CuratorServiceParams>(argc, argv);
}
