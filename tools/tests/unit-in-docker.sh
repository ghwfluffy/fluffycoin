#!/bin/bash

set -eu -o pipefail

TOP_DIR="$(dirname "$(dirname "$(dirname "$(readlink -f "${0}")")")")"

# Get tag
source "${TOP_DIR}/.env"

# Run docker
docker run \
    -ti \
    -v "${TOP_DIR}:${TOP_DIR}" \
    -v /etc/passwd:/etc/passwd:ro \
    -v /proc/cpuinfo:/proc/cpuinfo:ro \
    -u $(id -u):$(id -g) \
    --entrypoint "${TOP_DIR}/dist/fluffycoin-gtest" \
    "fcbuildenv:${FLUFFYCOIN_TAG}" \
    "${@}"
