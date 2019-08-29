#!/usr/bin/env bash

set -eu

if [ "${TRAVIS_OS_NAME}" != "osx" ]; then
    echo "Not a MacOS X build; skipping installation"
    exit 0
fi

$(dirname $0)/install-sources.sh




