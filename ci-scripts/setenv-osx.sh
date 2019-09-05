#!/usr/bin/env bash

# An 'exit' command in this script will abort the CI process

set -e

if [ "${TRAVIS_OS_NAME}" != osx ]; then
    echo "Not a MacOS X build; skipping environment setting."
else
    export PATH="/usr/local/opt/bison/bin:$PATH"
fi

