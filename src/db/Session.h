#pragma once

#include <fluffycoin/db/Result.h>
#include <fluffycoin/db/DataResult.h>

#include <fluffycoin/async/func.h>

#include <fluffycoin/utils/Details.h>

#include <string>

namespace fluffycoin::db
{

/**
 * Database connection potentially tied to a transaction
 */
class Session
{
    public:
        Session() = default;
        Session(Session &&) = default;
        Session(const Session &) = default;
        Session &operator=(Session &&) = default;
        Session &operator=(const Session &) = default;
        ~Session() = default;

        void query(
            std::string query,
            Details &details,
            async::func<void(Result, Details &)> callback);

        void select(
            std::string query,
            Details &details,
            async::func<void(DataResult, Details &)> callback);
};

}
