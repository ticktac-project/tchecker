#! /usr/bin/env bash

REPOSITORY=pictavien/tchecker-ci
if test $# = 0; then
  TAG=latest
else
  TAG="$1"
fi

set -euv

docker build --target gcc-image -t ${REPOSITORY}:gcc-${TAG}  .
docker build --target clang-image -t ${REPOSITORY}:clang-${TAG}  .
docker push ${REPOSITORY}:gcc-${TAG}
docker push ${REPOSITORY}:clang-${TAG}

