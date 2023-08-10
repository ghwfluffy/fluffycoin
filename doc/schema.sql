-- [Genesis 0] -> [Reconciliation 0] ... -> [Reconciliation n]
--     -> [Reconciliation n, Shard 0, Block 0, Transaction 0]
--          First block of each start is number 1
--          Reconciliation block is shard 0 block 0

-- transaction_id = reconciliation.shard.block.transaction
-- block_id = reconciliation.shard.block

CREATE TABLE blocks
(
    reconciliation BIGINT NOT NULL,
    shard INT NOT NULL,
    block INT NOT NULL,

    block_id VARCHAR NOT NULL,

    PRIMARY KEY(reconciliation, shard, block)
);

CREATE TABLE transactions
(
    -- Hash of primary key
    transaction_id VARCHAR NOT NULL,

    reconciliation BIGINT NOT NULL,
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

CREATE TABLE validator_join_transactions
(
    transaction_id VARCHAR NOT NULL,

    coins BIGINT NOT NULL,
    fluffs INT NOT NULL,

    validator_address VARCHAR,
    validator_pubkey bytea,

    stake_wallet BIGINT NOT NULL,

    PRIMARY KEY(transaction_id)
);

CREATE TABLE validator_leave_transactions
(
    transaction_id VARCHAR NOT NULL,

    validator_address VARCHAR,

    stake_wallet BIGINT NOT NULL,

    PRIMARY KEY(transaction_id)
);

CREATE TABLE validator_rotate_transactions
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

    staked_address VARCHAR,
    staked_pubkey bytea,
    staked_coins BIGINT,
    staked_fluffs INT,

    PRIMARY KEY(id)
);

CREATE TABLE wallet_addresses
(
    wallet_id BIGINT NOT NULL,
    address VARCHAR NOT NULL,
    pubkey bitea,

    PRIMARY KEY(wallet_id, address)
)

CREATE TABLE validator_rewards
(
    reconciliation BIGINT NOT NULL,
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
