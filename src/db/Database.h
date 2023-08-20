#pragma once

#include <fluffycoin/db/Session.h>

#include <fluffycoin/async/Ret.h>

#include <memory>

namespace fluffycoin::db
{

struct DatabaseImpl;

/**
 * Create connections to the database to run queries
 */
class Database
{
    public:
        Database();
        Database(Database &&);
        Database(const Database &) = delete;
        Database &operator=(Database &&);
        Database &operator=(const Database &) = delete;
        ~Database();

        void connect(
            const std::string &params,
            Details &details);

        async::Ret<Session> newSession(Details &details);
        async::Ret<Session> newReadOnlySession(Details &details);

    private:
        std::unique_ptr<DatabaseImpl> impl;
};

}
