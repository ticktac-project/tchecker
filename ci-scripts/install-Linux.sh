#!/usr/bin/env bash

LIBBOOST_VERSION=1.81

set -eu

apt-get update
apt-get install -y valgrind
if ! apt-get install -y libboost${LIBBOOST_VERSION}-dev  \
     libboost-json${LIBBOOST_VERSION}-dev; then
  apt-get install -y libboost-dev
fi

$(dirname $0)/install-sources.sh
