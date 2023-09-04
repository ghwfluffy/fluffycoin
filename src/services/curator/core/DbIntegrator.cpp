#include <fluffycoin/curator/DbIntegrator.h>

#include <fluffycoin/alg/Address.h>

#include <fluffycoin/db/Binary.h>

#include <fluffycoin/block/decode.h>

using namespace fluffycoin;
using namespace fluffycoin::curator;

DbIntegrator::DbIntegrator(
    db::Database &database)
        : database(database)
{
    initialized = false;
    currentRecon = 0;
}

async::Ret<DbIntegrator::Result> DbIntegrator::init(
    db::Session &dbsession,
    Details &details)
{
    // Query what the most recent block was
    db::DataResult result = co_await dbsession.select(
        "SELECT reconciliation, shard, block, hash FROM imported_blocks",
        details);
    if (!details.isOk())
    {
        details.extendError(log::Init, "integrate_init",
            "Failed to select current import state.");
        co_return Result::DatabaseError;
    }

    // We need the genesis block
    if (result.getNumResults() == 0)
    {
        initialized = true;
        co_return Result::Success;
    }

    // Figure out what the last thing we imported was
    bool first = true;
    currentRecon = 0;
    lastImports.clear();
    while (result.next())
    {
        const db::IResultRow &row = result.cur();
        uint32_t lastRecon = row[0].UInt32();
        if (!first)
        {
            if (currentRecon != lastRecon)
            {
                details.setError(log::General, ErrorCode::ArgumentInvalid, "dbintegrate_init",
                    "Database state mismatches current reconciliation block ({} vs {}).",
                    currentRecon, lastRecon);
                co_return Result::InvalidState;
            }
        }
        else
        {
            first = false;
            currentRecon = lastRecon;
        }

        uint32_t shard = row[1].UInt32();
        ShardInfo &info = lastImports[shard];
        info.block = row[2].UInt32();
        block::decode(row[3].data(), info.hash);
    }

    // Genesis block imported, need first reconciliation block
    if (!lastImports.empty() && currentRecon == 0)
        currentRecon = 1;
}

async::Ret<DbIntegrator::Result> DbIntegrator::integrate(
    const block::Block &data,
    uint32_t recon,
    uint32_t shard,
    uint32_t block,
    Details &details)
{
    // Get a new database session
    db::Session dbsession = co_await database.newSession(details);
    if (!details.isOk())
    {
        details.extendError(log::Init, "dbintegrate_init",
            "Failed to create database session for block integration.");
        co_return Result::NoDatabase;
    }

    // Initialize what we're looking for next
    if (!initialized)
    {
        Result status = co_await init(dbsession, details);
        if (status != Result::Success)
            co_return status;
    }

    // Is this the next recon we are waiting for?
    if (recon > currentRecon)
    {
        details.log().traffic(
            log::General,
            "Block for reconciliation {} too early (expecting {}).",
            recon, currentRecon);
        co_return Result::NotReady;
    }
    else if (recon < currentRecon)
    {
        details.log().traffic(
            log::General,
            "Blocks for reconciliation {} already complete (current recon {}).",
            recon, currentRecon);
        co_return Result::AlreadyDone;
    }

    // We need the genesis block
    if (currentRecon == 0)
    {
        // Only one genesis block
        if (shard != 0)
        {
            details.setError(log::General, ErrorCode::ArgumentInvalid, "dbintegrate",
                "Invalid genesis block shard {}.", shard);
            co_return Result::InvalidIndex;
        }
        else if (block != 0)
        {
            details.setError(log::General, ErrorCode::ArgumentInvalid, "dbintegrate",
                "Invalid genesis block number {}.", block);
            co_return Result::BadBlock;
        }
        else if (data.getType() != block::Block::Type::Genesis)
        {
            details.setError(log::General, ErrorCode::ArgumentInvalid, "dbintegrate",
                "Invalid genesis block type '{}'.", to_string(data.getType()));
            co_return Result::BadBlock;
        }

        DbIntegrator::Result ret = co_await integrateGenesis(data, dbsession, details);
        co_return ret;
    }

    // If this is reconciliation, does it say we've imported all the previous blocks
    if (data.getType() == block::Block::Type::Reconciliation)
    {
        // Does this recon match all of the imported shard hashes
        const block::Reconciliation &reconObj = *data.getReconciliation();
        for (size_t ui = 0; ui < reconObj.getShardInfo().size(); ui++)
        {
            const block::ReconciliationShardInfo &shardInfo = reconObj.getShardInfo()[ui];

            uint32_t blocksImported = 0;
            auto iterImported = lastImports.find(static_cast<uint32_t>(ui));
            if (iterImported != lastImports.end())
                blocksImported = iterImported->second.block;

            // Not enough blocks in shard
            if (blocksImported < shardInfo.blocks)
            {
                details.log().traffic(
                    log::General,
                    "Reconciliation block {} expects {} blocks in shard {} but only {} imported.",
                    recon, shardInfo.blocks, ui, blocksImported);
                co_return Result::NotReady;
            }
            // Too many blocks in shard
            else if (blocksImported > shardInfo.blocks)
            {
                details.setError(log::General, ErrorCode::ArgumentInvalid, "import_reconciliation",
                    "Reconciliation block {} expects {} blocks in shard {} but {} already imported.",
                    recon, shardInfo.blocks, ui, blocksImported);
                co_return Result::BadBlock;
            }
            // Expect null hash
            else if (iterImported == lastImports.end() && shardInfo.hash != block::Hash())
            {
                details.setError(log::General, ErrorCode::ArgumentInvalid, "import_reconciliation",
                    "Reconciliation block {} null hash mismatch on shard {}.",
                    recon, ui);
                co_return Result::BadBlock;
            }
            // Expect matching hash
            else if (iterImported != lastImports.end() && shardInfo.hash != iterImported->second.hash)
            {
                details.setError(log::General, ErrorCode::ArgumentInvalid, "import_reconciliation",
                    "Reconciliation block {} hash mismatch on shard {}.",
                    recon, ui);
                co_return Result::BadBlock;
            }
        }

        // Integrate reconciliation block
        DbIntegrator::Result ret = co_await integrateRecon(data, dbsession, details);
        co_return ret;
    }

    // Finally must be a normal node block
    if (data.getType() != block::Block::Type::Node)
    {
        details.setError(log::General, ErrorCode::ArgumentInvalid, "dbintegrate", "Invalid block type.");
        co_return Result::BadBlock;
    }

    Result ret = co_await integrateNode(data, shard, block, dbsession, details);
    co_return ret;
}

async::Ret<DbIntegrator::Result> DbIntegrator::integrateGenesis(
    const block::Block &data,
    db::Session &dbsession,
    Details &details)
{
    // Log info
    const block::Genesis &genesis = *data.getGenesis();

    details.log().info(log::General, "Integrating genesis block for chain '{}' version {}.",
        genesis.getName(), genesis.getVersion());

    details.log().info(log::General, "Chain created at {} with {} coins greed.",
        to_string(genesis.getCreation()), to_string(genesis.getGreed()));

    // Add validator address to database
    // Genesis block implicitly stakes 1 coin
    std::string query;
    query = "INSERT INTO validator_joins "
            "(transaction_id, coins, fluffs, validator_address, validator_pubkey, stake_wallet) ";
    query += fmt::format("VALUES ('0', 1, 0, '{}', {}, 1)",
        alg::Address::printable(genesis.getCreator().get()),
        db::Binary::encode(genesis.getValidatorKey().get()));
    co_await dbsession.query(query, details);
    if (!details.isOk())
    {
        details.extendError(log::General, "import_genesis", "Failed to add creator validator join.");
        co_return Result::DatabaseError;
    }

    // Add greed wallet
    query = "INSERT INTO wallets "
            "(id, coins, fluffs, staked_coins, staked_fluffs)";
    query += fmt::format("VALUES (1, {}, {}, 1, 0)",
        genesis.getGreed().getCoins() - 1,
        genesis.getGreed().getFluffs());
    co_await dbsession.query(query, details);
    if (!details.isOk())
    {
        details.extendError(log::General, "import_genesis", "Failed to add creator wallet.");
        co_return Result::DatabaseError;
    }

    // Add greed wallet initial address
    query = "INSERT INTO wallet_addresses "
            "(wallet_id, address, pubkey) ";
    query += fmt::format("VALUES (1, '{}', NULL)",
        alg::Address::printable(genesis.getStakeAddress().get()));
    co_await dbsession.query(query, details);
    if (!details.isOk())
    {
        details.extendError(log::General, "import_genesis", "Failed to add creator wallet address.");
        co_return Result::DatabaseError;
    }

    // Hash for import table
    BinData encoded = genesis.encode();
    block::Hash asn1Hash = block::Hash(encoded);
    BinData hash = asn1Hash.encode();

    query = "INSERT INTO imported_blocks "
            "(reconciliation, shard, block, hash) ";
    query += fmt::format("VALUES (0,0,0,{})", db::Binary::encode(hash));
    co_await dbsession.query(query, details);
    if (!details.isOk())
    {
        details.extendError(log::General, "import_genesis", "Failed to update import state.");
        co_return Result::DatabaseError;
    }

    co_await dbsession.commit(details);
    // TODO: Check details

    // Update cached state
    currentRecon = 1;
    ShardInfo genesisShard;
    genesisShard.hash = std::move(asn1Hash);
    lastImports.emplace(0, std::move(genesisShard));

    co_return Result::Success;
}

async::Ret<DbIntegrator::Result> DbIntegrator::integrateRecon(
    const block::Block &data,
    db::Session &dbsession,
    Details &details)
{
    const block::Reconciliation &recon = *data.getReconciliation();

    // XXX: Move check up one level?
    if (recon.getChainId() != currentRecon)
    {
        details.setError(log::General, ErrorCode::ArgumentInvalid, "import_reconciliation",
            "Reconciliation chain ID {} does not match metadata {}.", recon.getChainId(), currentRecon);
        co_return Result::BadBlock;
    }

    // XXX: Move check up one level?
    if (recon.getShardInfo().size() != lastImports.size())
    {
        details.setError(log::General, ErrorCode::ArgumentInvalid, "import_reconciliation",
            "Reconciliation block shard hashes does not match ({} vs {}).",
            recon.getShardInfo().size(),
            lastImports.size());
        co_return Result::BadBlock;
    }

    // This is a back to back recon block, or the recon after the genesis block
    for (size_t shardIndex = 0; shardIndex < recon.getShardInfo().size(); shardIndex++)
    {
        auto iterLastImport = lastImports.find(static_cast<uint32_t>(shardIndex));
        // XXX: This should not be able to happen if we got this far
        if (iterLastImport == lastImports.end())
        {
            details.setError(log::General, ErrorCode::ArgumentInvalid, "import_reconciliation",
                "Missing import for shard {}.", shardIndex);
            co_return Result::NoMatch;
        }

        const block::ReconciliationShardInfo &shardInfo = recon.getShardInfo()[shardIndex];
        const ShardInfo &importedInfo = iterLastImport->second;
        if (importedInfo.block != shardInfo.blocks)
        {
            details.setError(log::General, ErrorCode::ArgumentInvalid, "import_reconciliation",
                "Reconciliation shard {} expects {} blocks but {} imported.",
                shardIndex, shardInfo.blocks, importedInfo.block);
            co_return Result::NotReady;
        }

        // XXX: Again shouldnt this be done on the validator layer?
        //      Or is it good to have a failsafe at the db serialize layer?
        //      Think this will end up being redundant
        if (importedInfo.hash != shardInfo.hash)
        {
            details.setError(log::General, ErrorCode::ArgumentInvalid, "import_reconciliation",
                "Reconciliation block shard hash mismatch for shard {}.", shardIndex);
            co_return Result::BadBlock;
        }
    }

    // Log info
    details.log().info(log::General, "Integrating reconciliation block {}.", recon.getChainId());

    // Update DB:
    // Clear imported_blocks table
    co_await dbsession.query("TRUNCATE imported_blocks", details);
    if (!details.isOk())
    {
        details.extendError(log::General, "truncate_imported_blocks", "Failed to clear imported_blocks table.");
        co_return Result::DatabaseError;
    }

    // Add imported_blocks for recon
    // Hash for import table
    BinData encoded = recon.toContent();
    block::Hash asn1Hash = block::Hash(encoded);
    BinData hash = asn1Hash.encode();

    std::string query;
    query = "INSERT INTO imported_blocks "
            "(reconciliation, shard, block, hash) ";
    query += fmt::format("VALUES ({},0,0,{})", currentRecon, db::Binary::encode(hash));
    co_await dbsession.query(query, details);
    if (!details.isOk())
    {
        details.extendError(log::General, "import_reconciliation", "Failed to update import state.");
        co_return Result::DatabaseError;
    }

    co_await dbsession.commit(details);
    // TODO: Check details

    // Update cache:
    // Increment currentRecon
    currentRecon++;

    // Clear imported blocks and add lastImports for shard 0
    ShardInfo reconShard;
    reconShard.hash = std::move(asn1Hash);
    lastImports.clear();
    lastImports.emplace(0, std::move(reconShard));

    co_return Result::Success;
}

async::Ret<DbIntegrator::Result> DbIntegrator::integrateNode(
    const block::Block &data,
    uint32_t shard,
    uint32_t block,
    db::Session &dbsession,
    Details &details)
{
    const block::Node &node = *data.getNode();
    (void)node; // TODO
    (void)dbsession; // TODO

    uint32_t lastBlock = 0;
    auto iterLastImport = lastImports.find(shard);
    if (iterLastImport != lastImports.end())
        lastBlock = iterLastImport->second.block;

    // We're passed this point
    if (lastBlock >= block)
    {
        details.setError(log::General, ErrorCode::ArgumentInvalid, "import_node", "Block already integrated.");
        co_return Result::AlreadyDone;
    }

    // We're not that far yet
    if (lastBlock + 1 < block)
    {
        details.log().traffic(log::General,
            "Not ready for block {} in shard {} on recon {} (want {}).",
            block, shard, currentRecon, lastBlock + 1);
        co_return Result::NotReady;
    }

    co_await dbsession.commit(details);
    // TODO: Check details

    // TODO: For each transfer transaction:
    // modify the coins in the sender/receiver wallets
    // link the new sender address to the wallet id

    // TODO: block==1: Insert into imported_blocks, else: Update imported_blocks
    // TODO: Append lastImported cached state

    co_return Result::Success;
}
