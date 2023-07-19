#!/usr/bin/env bash

LIBBOOST_VERSION=1.81

set -eu

sudo apt-get update
sudo apt-get install valgrind \
    libboost${LIBBOOST_VERSION}-dev  \
    libboost-json${LIBBOOST_VERSION}-dev

$(dirname $0)/install-sources.sh
