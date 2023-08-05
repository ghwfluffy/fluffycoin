#include <fluffycoin/validator/ValidatorServiceParams.h>

#include <fluffycoin/svc/Run.h>

using namespace fluffycoin;
using namespace fluffycoin::validator;

int main(int argc, const char *argv[])
{
    return svc::run<ValidatorServiceParams>(argc, argv);
}
