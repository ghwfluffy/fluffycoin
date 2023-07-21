#!/bin/bash

set -eu -o pipefail

JOBS="$(grep processor /proc/cpuinfo | wc -l)"
usage() {
    echo "Usage: $(basename "${0}") <Options>

            -v --verbose            Verbose output
            -d --debug              Debug build
            -j --jobs <Value>       Number of build threads (Default: ${JOBS})

            -c --clean              Clean run
            -C --clean-only         Clean and exit" 1>&2
}

CLEAN=0
VERBOSE=""
BUILD_TYPE="-DCMAKE_BUILD_TYPE=Release"
while [ $# -gt 0 ]; do
    case "${1}" in
        -v|--verbose)
            set -x
            VERBOSE="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="-DCMAKE_BUILD_TYPE=Debug"
            shift
            ;;
        -j|--jobs)
            JOBS="${2}"
            shift
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -c|--clean)
            CLEAN=1
            shift
            ;;
        -C|--clean-only)
            CLEAN=2
            shift
            ;;
        *)
            echo "Unknown argument ${1}" 1>&2
            usage
            exit 1
            ;;
    esac
done

# Directories
TOP_DIR="$(dirname "$(dirname "$(dirname "$(readlink -f "${0}")")")")"
BUILD_DIR="${TOP_DIR}/.cmake"
DIST_DIR="${TOP_DIR}/dist"

# Cleanup previous build
if [ ${CLEAN} -gt 0 ]; then
    rm -rf "${BUILD_DIR}" "${DIST_DIR}"
    if [ ${CLEAN} -gt 1 ]; then
        exit 0
    fi
fi

# Build
mkdir -p "${BUILD_DIR}"
(
    cd "${BUILD_DIR}"
    cmake \
        ${VERBOSE} \
        ${BUILD_TYPE} \
        -G Ninja \
        "${TOP_DIR}"
    cmake --build . \
        -j ${JOBS}
)
