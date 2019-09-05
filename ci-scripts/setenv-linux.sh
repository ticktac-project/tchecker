#!/usr/bin/env bash

# An 'exit' command in this script will abort the CI process

set -e

if [ "${TRAVIS_OS_NAME}" != linux ]; then
    echo "Not a Linux build; skipping environment setting."
else
    :
fi
