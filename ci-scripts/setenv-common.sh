#!/usr/bin/env bash

export INSTALL_DIR=${HOME}/install
export BUILD_TYPE=Debug
export NB_CPUS=$(getconf _NPROCESSORS_CONF)

if test -z "${NB_CPUS}"; then
    NB_CPUS=2
fi

export CTEST_PARALLEL_LEVEL=${NB_CPUS}
