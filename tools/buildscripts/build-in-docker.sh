#!/bin/bash

set -eu -o pipefail

# Dependent on build.sh
BUILDSCRIPT_DIR="$(dirname $(readlink -f "${0}"))"
BUILDSCRIPT="${BUILDSCRIPT_DIR}/build.sh"

# Arguments
usage() {
    echo "Usage: $(basename "${0}") <Options> [build.sh options]

            -X --no-cache           Don't use docker cache
            -D --drop-in            Drop into build env
" 1>&2

    echo "build.sh options" 1>&2
    "${BUILDSCRIPT}" --help
}

CACHE=""
ENTRYPOINT="${BUILDSCRIPT}"
while [ $# -gt 0 ]; do
    case "${1}" in
        -X|--no-cache)
            CACHE="--no-cache"
            shift
            ;;
        -D|--drop-in)
            ENTRYPOINT="/bin/bash"
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            break
            ;;
    esac
done

# Directories
TOP_DIR="$(dirname "$(dirname "${BUILDSCRIPT_DIR}")")"
DOCKER_DIR="${TOP_DIR}/docker/buildenv"

# Get tag
source "${TOP_DIR}/.env"

# Build docker
(
    cd "${TOP_DIR}"
    docker build \
        ${CACHE} \
        "${DOCKER_DIR}" \
        -t "fluffyco.in/buildenv:${FLUFFYCOIN_TAG}"
)

# Run docker
docker run \
    -ti \
    -v "${TOP_DIR}:${TOP_DIR}" \
    -v /etc/passwd:/etc/passwd:ro \
    -v /proc/cpuinfo:/proc/cpuinfo:ro \
    -u $(id -u):$(id -g) \
    --entrypoint "${ENTRYPOINT}" \
    "fluffyco.in/buildenv:${FLUFFYCOIN_TAG}" \
    "${@}"
