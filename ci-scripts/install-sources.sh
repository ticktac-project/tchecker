#!/usr/bin/env bash

set -eu

echo "#### INSTALLING Catch2"

git clone --branch ${CATCH2_BRANCH} https://github.com/catchorg/Catch2.git ${CATCH2_REPO} || true
mkdir -p ${CATCH2_REPO}/build || true

pushd ${CATCH2_REPO}/build
 cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ..
 make -j ${NB_CPUS}
 make install
 ls -R ${INSTALL_DIR}
popd



