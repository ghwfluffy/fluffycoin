#pragma once

#include <fluffycoin/db/Session.h>

#include <fluffycoin/async/func.h>

namespace fluffycoin::db
{

class Database
{
    public:
        virtual ~Database() = default;

        void newReadOnlySession(
            Details &details,
            async::func<void(Session session, Details &details)> callback);

    protected:
        Database() = default;
        Database(Database &&) = default;
        Database(const Database &) = default;
        Database &operator=(Database &&) = default;
        Database &operator=(const Database &) = default;
};

}
