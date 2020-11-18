#! /usr/bin/env bash

REPOSITORY=pictavien/tchecker-ci
if test $# = 0; then
  TAG=latest
else
  TAG="$1"
fi

set -euv

docker build --build-arg IMAGE=gcc:9 \
       --build-arg CXX=g++\
       -t ${REPOSITORY}:gcc-${TAG}  .
docker push ${REPOSITORY}:gcc-${TAG}

docker build --build-arg IMAGE=debian:stable --build-arg DEBPKG=clang \
       --build-arg CXX=clang++\
       -t ${REPOSITORY}:clang-${TAG}  .
docker push ${REPOSITORY}:clang-${TAG}

