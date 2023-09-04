#!/bin/bash

set -eux -o pipefail

USER="postgres"

cleanup() {
    dropdb -U "${USER}" fluffytest &> /dev/null || true
}
trap cleanup EXIT
cleanup
createdb -U "${USER}" fluffytest
psql -U "${USER}" fluffytest < ../../doc/schema.sql
