#!/bin/bash

set -eu -o pipefail

TOP_DIR="$(dirname "$(dirname "$(dirname "$(readlink -f "${0}")")")")"

usage() {
    echo "Usage: $(basename "${0}") <Options>

            -g                      Run in GDB
            -v                      Run in valgrind memcheck
            -t <Value>              gtest_filter
            -D                      Don't run database integration tests
            -DD                     Dump postgres output"
}

ARGS=()
FILTER=""
DUMP_DB=0
TEST_DB=1
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
            shift
            ;;
        -D|--no-database)
            TEST_DB=0
            shift
            ;;
        -DD|--debug-database)
            DUMP_DB=1
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

# Get tag
source "${TOP_DIR}/.env"

# Cleanup all the dockers we start
cleanup() {
    docker stop fcgtest 2> /dev/null || true
    docker stop fcpgtest 2> /dev/null || true
    docker network rm fc_testnetwork 2> /dev/null || true
}
trap cleanup EXIT
cleanup

echo "Setup docker network"
docker network create fc_testnetwork

# Running database integration tests?
EXCLUDE=""
if [ ${TEST_DB} -eq 0 ]; then
    # Any test with Postgres in the name gets skipped
    EXCLUDE="--gtest_filter=-*Postgres*"
else
    echo "Run postgres container"
    docker run \
        --rm \
        --name fcpgtest \
        --network fc_testnetwork \
        -e POSTGRES_PASSWORD=fluffy \
        -v "${TOP_DIR}/tools/tests/test.sql":/docker-entrypoint-initdb.d/init-db.sql \
        -d \
        postgres:15-alpine3.18

    # Wait for postgres to start
    # Not very scientific
    sleep 3
fi

RET=0
echo "Run tests"
docker run \
    -ti \
    --rm \
    --name fcgtest \
    --network fc_testnetwork \
    -v "${TOP_DIR}:${TOP_DIR}" \
    -v /etc/passwd:/etc/passwd:ro \
    -v /proc/cpuinfo:/proc/cpuinfo:ro \
    -u $(id -u):$(id -g) \
    -e PGUSER=fluffy \
    -e PGPASSWORD=fluffy \
    -e PGCONNECT="host=fcpgtest dbname=fluffycoin" \
    --entrypoint "${ENTRYPOINT}" \
    "fluffyco.in/tests:${FLUFFYCOIN_TAG}" \
    "${ARGS[@]}" \
    ${EXCLUDE} \
    ${FILTER} || RET=$?

# Dump postgres output
if [ ${TEST_DB} -eq 1 ] && [ ${DUMP_DB} -eq 1 ]; then
    docker logs fcpgtest
fi

exit ${RET}
