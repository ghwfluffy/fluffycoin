#include <fluffycoin/p2p/ValidatorLookup.h>

using namespace fluffycoin;
using namespace fluffycoin::p2p;

const BinData &ValidatorInfo::getPubKey() const
{
    // TODO
    static BinData todo;
    return todo;
}

bool ValidatorInfo::isActive() const
{
    // TODO
    return false;
}

ValidatorInfo ValidatorLookup::getValidator(
    const std::string &address,
    Details &details)
{
    // TODO
    (void)address;
    details.setError(log::Auth, ErrorCode::InternalError, "p2p_lookup", "Not implemented");
    return ValidatorInfo();
}
