#pragma once

#include <fluffycoin/db/Result.h>
#include <fluffycoin/db/DataResult.h>

#include <fluffycoin/async/Ret.h>

#include <fluffycoin/utils/Details.h>

#include <string>
#include <memory>

namespace fluffycoin::db
{

namespace priv {
struct SessionImpl;
}

/**
 * Database connection potentially tied to a transaction
 */
class Session
{
    public:
        Session();
        Session(std::unique_ptr<priv::SessionImpl> impl);
        Session(Session &&);
        Session(const Session &) = delete;
        Session &operator=(Session &&);
        Session &operator=(const Session &) = delete;
        ~Session();

        async::Ret<Result> query(
            std::string query,
            Details &details);

        async::Ret<DataResult> select(
            std::string query,
            Details &details);

    private:
        std::unique_ptr<priv::SessionImpl> impl;
};

}
