#pragma once

#include <fluffycoin/db/Database.h>

#include <fluffycoin/async/Ret.h>

#include <fluffycoin/block/Block.h>

#include <map>

namespace fluffycoin::curator
{

/**
 * Takes new blocks and tries to integrate them into the database state.
 */
class DbIntegrator
{
    public:
        DbIntegrator(
            db::Database &database);
        DbIntegrator(DbIntegrator &&) = delete;
        DbIntegrator(const DbIntegrator &) = delete;
        DbIntegrator &operator=(DbIntegrator &&) = delete;
        DbIntegrator &operator=(const DbIntegrator &) = delete;
        ~DbIntegrator() = default;

        enum class Result
        {
            None = 0,
            Success,        /* <! Successfully integrated with database */
            NoDatabase,     /* <! Not able to access the database */
            DatabaseError,  /* <! Database error occurred */
            InvalidState,   /* <! Database state is not valid */
            InvalidIndex,   /* <! The block index is not valid */
            Duplicate,      /* <! This block index already integrated */
            BadBlock,       /* <! Not able to interpret the block data */
            NoMatch,        /* <! Block does not match the previously integrated block */
            NotReady,       /* <! It is too early to integrate this block (Missing previous blocks) */
            AlreadyDone,    /* <! This block index has already been integrated before */
        };

        async::Ret<Result> integrate(
            const block::Block &data,
            uint32_t recon,
            uint32_t shard,
            uint32_t block,
            Details &details);

    private:
        async::Ret<Result> init(
            db::Session &dbsession,
            Details &details);

        async::Ret<Result> integrateGenesis(
            const block::Block &data,
            db::Session &session,
            Details &details);

        async::Ret<Result> integrateRecon(
            const block::Block &data,
            db::Session &session,
            Details &details);

        async::Ret<Result> integrateNode(
            const block::Block &data,
            uint32_t shard,
            uint32_t block,
            db::Session &session,
            Details &details);

        db::Database &database;

        bool initialized;
        uint32_t currentRecon;

        struct ShardInfo
        {
            uint32_t block = 0;
            block::Hash hash;
        };
        std::map<uint32_t, ShardInfo> lastImports;
};

}
