#!/usr/bin/env bash

set -eu

brew update
brew upgrade
brew install boost
brew install bison@3.8

$(dirname $0)/install-sources.sh




