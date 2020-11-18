#!/usr/bin/env bash

set -euv

NB_CPUS=$(getconf _NPROCESSORS_CONF)
CTEST_PARALLEL_LEVEL=${NB_CPUS} CTEST_OUTPUT_ON_FAILURE=1 make -C build test

