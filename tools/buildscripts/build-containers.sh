#!/bin/bash

set -eu -o pipefail

# Arguments
usage() {
    echo "Usage: $(basename "${0}") <Options>

            -X --no-cache           Don't use docker cache
            -L --not-latest         Don't tag images as 'latest'" 1>&2
}

CACHE=""
function latest() { echo "-t ${1}:latest"; }
while [ $# -gt 0 ]; do
    case "${1}" in
        -X|--no-cache)
            CACHE="--no-cache"
            shift
            ;;
        -L|--not-latest)
            function latest() { echo -n ""; }
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument ${1}" 1>&2
            usage
            exit 1
            ;;
    esac
done

# Directories
BUILDSCRIPT_DIR="$(dirname $(readlink -f "${0}"))"
TOP_DIR="$(dirname "$(dirname "${BUILDSCRIPT_DIR}")")"
DOCKER_DIR="${TOP_DIR}/docker"

# Get tag
source "${TOP_DIR}/.env"

# Build docker
export DOCKER_BUILDKIT=1
export WORKDIR="${TOP_DIR}"

# Validator node containers
for container in p2p curator; do
    docker build \
        ${CACHE} \
        "${TOP_DIR}" \
        -f "${DOCKER_DIR}/validator/${container}/Dockerfile" \
        -t "fluffyco.in/${container}:${FLUFFYCOIN_TAG}" \
        `latest "fluffyco.in/${container}"`
done

# CLI runtime
docker build \
    ${CACHE} \
    "${TOP_DIR}" \
    -f "${DOCKER_DIR}/cli/Dockerfile" \
    -t "fluffyco.in/cli:${FLUFFYCOIN_TAG}" \
    `latest "fluffyco.in/cli"`

# Unit and integration tests
docker build \
    ${CACHE} \
    "${TOP_DIR}" \
    -f "${DOCKER_DIR}/tests/Dockerfile" \
    --build-arg="FLUFFYCOIN_TAG=${FLUFFYCOIN_TAG}" \
    -t "fluffyco.in/tests:${FLUFFYCOIN_TAG}" \
    `latest "fluffyco.in/tests"`

# Postgres
docker pull postgres:15-alpine3.18
