#!/usr/bin/env bash

set -uv

NB_CPUS=$(getconf _NPROCESSORS_CONF)
if test -z "$NB_CPUS"; then
  NB_CPUS=2
fi

CTEST_PARALLEL_LEVEL=${NB_CPUS} CTEST_OUTPUT_ON_FAILURE=1 make -C build test
res=$?
if test ${res} != 0; then
  find build/Testing -name '*.log' -print -exec cat {} \;
fi
exit ${res}
