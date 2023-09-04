#!/bin/bash

set -eu -o pipefail

cat << CLI
Fluffycoin Command Line Tools

fluffycoin-genesis: Create a new genesis block
fluffycoin-wallet: Create and manage wallet keys
fluffycoin-info: Query information from the validators
fluffycoin-download: Download all or part of the binary blockchain
fluffycoin-validator: Sign up as a validator, and other validator actions
fluffycoin-lottery: Buy a lottery ticket
fluffycoin-transfer: Submit a transfer transaction
fluffycoin-trinket: Submit a trinket transaction
fluffycoin-loans: Submit actions related to loans
fluffycoin-vote: Submit actions related to votes
fluffycoin-stocks: Submit actions related to stocks
CLI

# Persistent data location
if [ ! -v FLUFFYCOIN_RESOURCE_DIR ]; then
    if [ -v HOME ]; then
        FLUFFYCOIN_RESOURCE_DIR="${HOME}/.fluffycoin"
    else
        FLUFFYCOIN_RESOURCE_DIR="/"
    fi
fi

cd "${FLUFFYCOIN_RESOURCE_DIR}" &> /dev/null || true

exec /bin/bash
