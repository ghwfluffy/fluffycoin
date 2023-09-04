-- [Genesis 0] -> [Reconciliation 1] ... -> [Reconciliation n]
--     -> [Reconciliation n, Shard 0, Block 0, Transaction 0]
--          First block of each shard is number 1
--          Reconciliation block is shard 0 block 0

-- transaction_id = reconciliation.shard.block.transaction
-- block_id = reconciliation.shard.block

-- Genesis=0,0,0
-- First Recon=1,0,0
-- First shard first block=1,1,1
-- nth Recon = n,0,0

BEGIN WORK;

-- One row per shard, what is the most recent thing we imported
-- We don't start importing the next reconciliation till the whole previous one is imported
CREATE TABLE imported_blocks
(
    reconciliation INT NOT NULL,
    shard INT NOT NULL,
    block INT NOT NULL,
    -- block::asn1::Hash
    hash bytea NOT NULL,

    PRIMARY KEY (reconciliation, shard)
);

-- Every vote from a validator
CREATE TABLE validations
(
    reconciliation INT NOT NULL,
    shard INT NOT NULL,
    block INT NOT NULL,

    validator_address VARCHAR NOT NULL,
    -- If they votes the block is good
    vote BOOLEAN NOT NULL,

    PRIMARY KEY(reconciliation, shard, block, validator_address)
);

-- Every transaction in a block
CREATE TABLE transactions
(
    -- Hash of primary key
    transaction_id VARCHAR NOT NULL,

    reconciliation INT NOT NULL,
    shard INT NOT NULL,
    block INT NOT NULL,
    transaction_index INT NOT NULL,

    -- Enum to determine x_transactions subtable
    transaction_type INT NOT NULL,

    -- Time created
    created BIGINT NOT NULL,

    -- Transaction fee
    catnip_coins BIGINT NOT NULL,
    catnip_fluffs INT NOT NULL,

    PRIMARY KEY(reconciliation, shard, block, transaction_index)
);

-- Every transfer transaction
CREATE TABLE transfer_transactions
(
    transaction_id VARCHAR NOT NULL,

    source_wallet BIGINT NOT NULL,
    source_address VARCHAR NOT NULL,
    updated_address VARCHAR NOT NULL,

    dest_wallet BIGINT NOT NULL,
    dest_address VARCHAR NOT NULL,

    coins BIGINT NOT NULL,
    fluffs INT NOT NULL,

    PRIMARY KEY(transaction_id)
);

-- Every validator onboarding
CREATE TABLE validator_joins
(
    transaction_id VARCHAR NOT NULL,

    coins BIGINT NOT NULL,
    fluffs INT NOT NULL,

    validator_address VARCHAR,
    validator_pubkey bytea,

    stake_wallet BIGINT NOT NULL,

    PRIMARY KEY(transaction_id)
);

-- Every validator offboarding
CREATE TABLE validator_leaves
(
    transaction_id VARCHAR NOT NULL,

    validator_address VARCHAR,

    stake_wallet BIGINT NOT NULL,

    PRIMARY KEY(transaction_id)
);

-- Every validator key rotation
CREATE TABLE validator_rotates
(
    transaction_id VARCHAR NOT NULL,

    -- Delta
    coins BIGINT NOT NULL,
    fluffs INT NOT NULL,

    validator_address VARCHAR,
    validator_pubkey bytea,

    stake_wallet BIGINT NOT NULL,

    PRIMARY KEY(transaction_id)
);

-- Wallets: Chains of addresses
CREATE TABLE wallets
(
    id BIGINT NOT NULL,

    coins BIGINT NOT NULL,
    fluffs INT NOT NULL,

    staked_coins BIGINT,
    staked_fluffs INT,

    PRIMARY KEY(id)
);

CREATE TABLE wallet_addresses
(
    wallet_id BIGINT NOT NULL,
    address VARCHAR NOT NULL,
    pubkey bytea,

    PRIMARY KEY(wallet_id, address)
);

CREATE TABLE validator_rewards
(
    reconciliation INT NOT NULL,
    shard INT NOT NULL,
    block INT NOT NULL,

    -- Nominated block signer
    validator_address VARCHAR NOT NULL,

    -- Reward for the nominated validator
    validator_catnip_coins BIGINT NOT NULL,
    validator_catnip_fluffs INT NOT NULL,

    -- Reward for all other validators (per validator)
    others_catnip_coins BIGINT NOT NULL,
    others_catnip_fluffs INT NOT NULL,

    PRIMARY KEY(reconciliation, shard, block)
);

-- Database patches that have been applied
CREATE TABLE patches
(
    patch VARCHAR NOT NULL,
    PRIMARY KEY (patch)
);
INSERT INTO patches (patch) VALUES ('init-1.0.0');

COMMIT;
