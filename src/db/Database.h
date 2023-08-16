#pragma once

#include <fluffycoin/db/Session.h>

#include <fluffycoin/async/func.h>

namespace fluffycoin::db
{

/**
 * Create connections to the database to run queries
 */
class Database
{
    public:
        Database() = default;
        Database(Database &&) = default;
        Database(const Database &) = default;
        Database &operator=(Database &&) = default;
        Database &operator=(const Database &) = default;
        ~Database() = default;

        void newReadOnlySession(
            Details &details,
            async::func<void(Session, Details &)> callback);

        void newSession(
            Details &details,
            async::func<void(Session, Details &)> callback);
};

}
