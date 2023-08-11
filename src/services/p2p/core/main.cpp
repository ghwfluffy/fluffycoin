#include <fluffycoin/p2p/P2pServiceParams.h>

#include <fluffycoin/svc/Run.h>

using namespace fluffycoin;
using namespace fluffycoin::p2p;

int main(int argc, const char *argv[])
{
    return svc::run<P2pServiceParams>(argc, argv);
}
