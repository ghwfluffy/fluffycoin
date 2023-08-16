#include <fluffycoin/db/Database.h>

using namespace fluffycoin;
using namespace fluffycoin::db;

void Database::newReadOnlySession(
    Details &details,
    async::func<void(Session, Details &)> callback)
{
    // TODO
    (void)details;
    (void)callback;
}

void Database::newSession(
    Details &details,
    async::func<void(Session, Details &)> callback)
{
    // TODO
    (void)details;
    (void)callback;
}
