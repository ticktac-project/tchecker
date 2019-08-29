#!/usr/bin/env bash

set -eu

if [ "${TRAVIS_OS_NAME}" != linux ]; then
    echo "Not a Linux build; skipping installation"
    exit 0
fi

$(dirname $0)/install-sources.sh


