#!/usr/bin/env bash

set -eu

sudo apt-get update
sudo apt-get install libboost-all-dev valgrind

$(dirname $0)/install-sources.sh


