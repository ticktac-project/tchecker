#!/usr/bin/env bash

set -euvx

mkdir build || true
cd build
cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      ${TEST_CONF} \
      ..

make -j ${NB_CPUS}

CTEST_OUTPUT_ON_FAILURE=1 make test

