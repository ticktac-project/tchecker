#!/bin/bash -x

SCRIPTDIR=$(cd $(dirname $0); pwd)
BUILDDIR=build/docker-${TCHECKER_DOCKER_IMAGE}
PROJECTROOT=$(cd ${SCRIPTDIR}/..; pwd)

error() {
  echo "$0: error: $1." 1>&2
  exit 1
}

${TCHECKER_DOCKER:-false} || error "not in a tcheker container"

cd ${PROJECTROOT}

export INSTALL_DIR=${BUILDDIR}/_inst
export TCHECKER_BUILD_TYPE=Debug
export CXX=g++
export CC=gcc
export CONFNAME=all:int64:memcheck
export VERBOSE=1
./ci-scripts/build.sh ${BUILDDIR}

