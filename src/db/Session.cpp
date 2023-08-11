#include <fluffycoin/db/Session.h>

using namespace fluffycoin;
using namespace fluffycoin::db;

void Session::query(
    std::string query,
    Details &details,
    async::func<void(Result, Details &)> callback)
{
    (void)query;
    (void)details;
    (void)callback;
    // TODO
}

void Session::select(
    std::string query,
    Details &details,
    async::func<void(DataResult, Details &)> callback)
{
    (void)query;
    (void)details;
    (void)callback;
    // TODO
}
