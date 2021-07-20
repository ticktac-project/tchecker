#!/usr/bin/env bash

set -eu

brew update
brew install boost
brew install bison@3.7

$(dirname $0)/install-sources.sh




