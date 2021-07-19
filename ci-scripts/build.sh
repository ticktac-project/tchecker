#!/usr/bin/env bash

if ! test -z "${DEBUG_BUILD}"; then
  set -v
fi

set -eu


nbbits="32"
memcheck="OFF"
algos="OFF"
unittests="OFF"
simple_nr="OFF"
bugfixes="OFF"

if test -z "${CONFNAME}"; then
  echo "Empty CONFNAME variable"
  exit 1
fi

echo "Build tchecker for configuration: ${CONFNAME}"

for cfgopt in $(sed -e 's/:/ /g' <<< "${CONFNAME}"); do
  case "${cfgopt}" in
    "int16"|"int32"|"int64")
      nbbits=${cfgopt/int/}
      ;;
    "memcheck")
      memcheck="ON"
      ;;
    "algos")
      algos="ON"
      ;;
    "all")
      algos="ON"; unittests="ON"; simple_nr="ON"; bugfixes="ON"
      ;;
    "smallest")
      unittests="ON"; simple_nr="ON"; bugfixes="ON"
      ;;
  esac
done

TEST_CONF="-DUSEINT${nbbits}=ON"
TEST_CONF="${TEST_CONF} -DTCK_ENABLE_MEMCHECK_TESTS=${memcheck}"
TEST_CONF="${TEST_CONF} -DTCK_ENABLE_ALGOS_TESTS=${algos}"
TEST_CONF="${TEST_CONF} -DTCK_ENABLE_UNITTESTS=${unittests}"
TEST_CONF="${TEST_CONF} -DTCK_ENABLE_SIMPLE_NR_TESTS=${simple_nr}"
TEST_CONF="${TEST_CONF} -DTCK_ENABLE_BUGFIXES_TESTS=${bugfixes}"

echo "TEST_CONF=${TEST_CONF}"

mkdir build || true
cd build
cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
      -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_BUILD_TYPE=${TCHECKER_BUILD_TYPE} \
      ${TEST_CONF} \
      ..

NB_CPUS=$(getconf _NPROCESSORS_CONF)
exec make -j ${NB_CPUS}

