#!/usr/bin/env bash

set -eu

brew update
brew upgrade
brew install boost@1.81
brew install bison@3.8

$(dirname $0)/install-sources.sh




