#!/usr/bin/env bash

set -eux

echo "#### INSTALLING Catch2"

git clone --branch ${CATCH2_BRANCH} ${CATCH2_REPO_URL} ${CATCH2_DIRECTORY} || true
mkdir -p ${CATCH2_DIRECTORY}/build || true

pushd ${CATCH2_DIRECTORY}/build
 cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ..
 make -j ${NB_CPUS}
 make install
 ls -R ${INSTALL_DIR}
popd



