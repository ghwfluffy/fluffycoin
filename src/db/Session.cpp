#include <fluffycoin/db/Session.h>
#include <fluffycoin/db/priv/SessionImpl.h>

using namespace fluffycoin;
using namespace fluffycoin::db;

Session::Session()
{
}

Session::Session(std::unique_ptr<priv::SessionImpl> impl)
        : impl(std::move(impl))
{
}

Session::Session(Session &&rhs)
        : impl(std::move(rhs.impl))
{
}

Session &Session::operator=(Session &&rhs)
{
    if (this != &rhs)
        impl = std::move(rhs.impl);

    return (*this);
}

Session::~Session()
{
}

async::Ret<Result> Session::query(
    std::string query,
    Details &details)
{
    (void)query;
    (void)details;
    co_return Result();
}

async::Ret<DataResult> Session::select(
    std::string query,
    Details &details)
{
    (void)query;
    (void)details;
    co_return DataResult();
}
