#!/bin/bash

set -eu -o pipefail

# Arguments
usage() {
    echo "Usage: $(basename "${0}") <Options>

            -X --no-cache           Don't use docker cache" 1>&2
}

CACHE=""
while [ $# -gt 0 ]; do
    case "${1}" in
        -X|--no-cache)
            CACHE="--no-cache"
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

docker build \
    ${CACHE} \
    "${TOP_DIR}" \
    -f "${DOCKER_DIR}/validator/Dockerfile" \
    -t "fcvalidator:${FLUFFYCOIN_TAG}"

docker build \
    ${CACHE} \
    "${TOP_DIR}" \
    -f "${DOCKER_DIR}/cli/Dockerfile" \
    -t "fccli:${FLUFFYCOIN_TAG}"
