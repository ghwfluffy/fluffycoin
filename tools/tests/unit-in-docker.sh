#!/bin/bash

set -eu -o pipefail

TOP_DIR="$(dirname "$(dirname "$(dirname "$(readlink -f "${0}")")")")"

usage() {
    echo "Usage: $(basename "${0}") <Options>

            -g                      Run in GDB
            -v                      Run in valgrind memcheck
            -t <Value>              gtest_filter"
}

ARGS=()
FILTER=""
ENTRYPOINT="${TOP_DIR}/dist/bin/fluffycoin-gtest"
while [ $# -gt 0 ]; do
    case "${1}" in
        -t|--gtest_filter)
            FILTER="--gtest_filter=*${2}*"
            shift
            shift
            ;;
        -g|--gdb)
            ARGS=("--args" "${ENTRYPOINT}")
            ENTRYPOINT="gdb"
            shift
            ;;
        -v|--valgrind)
            ARGS=(
                "--tool=memcheck"
                "--num-callers=50"
                "--leak-check=full"
                "--track-origins=yes"
                "-v"
                "${ENTRYPOINT}")
            ENTRYPOINT="valgrind"
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

# Get tag
source "${TOP_DIR}/.env"

# Run docker
docker run \
    -ti \
    -v "${TOP_DIR}:${TOP_DIR}" \
    -v /etc/passwd:/etc/passwd:ro \
    -v /proc/cpuinfo:/proc/cpuinfo:ro \
    -u $(id -u):$(id -g) \
    --entrypoint "${ENTRYPOINT}" \
    "fcunit-tests:${FLUFFYCOIN_TAG}" \
    "${ARGS[@]}" \
    ${FILTER}
