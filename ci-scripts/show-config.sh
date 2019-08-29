#!/usr/bin/env bash

set -e

env | sort

echo "##############################################################"
echo PATH=${PATH}
echo NB_CPUS=${NB_CPUS}

echo "Compiler configuration:"
echo CXX=${CXX}
echo CC=${CC}
echo CXXFLAGS=${CXXFLAGS}

echo "C++ compiler version:"
${CXX} --version || echo "${CXX} does not seem to support the --version flag"
${CXX} -v || echo "${CXX} does not seem to support the -v flag"

echo "C compiler version:"
${CC} --version || echo "${CXX} does not seem to support the --version flag"
${CC} -v || echo "${CXX} does not seem to support the -v flag"

echo "##############################################################"

exit 0
