#!/usr/bin/env bash

LIBBOOST_VERSION=1.81

set -eu

sudo apt-get update
sudo apt-get install -y valgrind
if ! sudo apt-get install -y libboost${LIBBOOST_VERSION}-dev  \
     libboost-json${LIBBOOST_VERSION}-dev; then
  sudo apt-get install -y libboost-dev
fi

$(dirname $0)/install-sources.sh
