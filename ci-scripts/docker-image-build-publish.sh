#! /usr/bin/env bash

GCC_LATEST_IMAGE="gcc-12"
CLANG_LATEST_IMAGE="clang-13"
REPOSITORY=pictavien/tchecker-ci

SCRIPTDIR=$(cd $(dirname $0); pwd)

OPT_TAG=NONE
OPT_GCC_IMAGE=NONE
OPT_CLANG_IMAGE=NONE

# option set to true if built images have to be pushed on Docker registry
OPT_PUSH=false
# option set to true to only display Docker commands
OPT_DRY=false

usage() {
  cat <<EOF
USAGE: $0 [options]
where options are:
  -n
  -p
  -g gcc-image
  -c clang-image
  -t image-tag-suffix
  -h display this help message

EOF
  exit 0
}

error() {
  echo "$0: error: $1." 1>&2
  exit 1
}

parse_args() {
  OPTIONS=":g:c:t:nph"
  while getopts ${OPTIONS} opt; do
    case ${opt} in
      "g") OPT_GCC_IMAGE=${OPTARG} ;;
      "c") OPT_CLANG_IMAGE=${OPTARG} ;;
      "t") OPT_TAG=${OPTARG} ;;
      "h") usage ;;
      "p") OPT_PUSH=true ;;
      "n") OPT_DRY=true ;;
      ":") error "missing argument to option '${OPTARG}'" ;;
      "?") error "invalid option '${OPTARG}'" ;;
    esac
  done

  if test $((${OPTIND}-1)) -ne $#; then
    error "wrong # of arguments ($#)"
  fi
}

set -eu

parse_args "$@"

GCC_COMPILER_IMAGE=${OPT_GCC_IMAGE}
CLANG_COMPILER_IMAGE=${OPT_CLANG_IMAGE}
case ${OPT_TAG} in
  "latest")
    GCC_COMPILER_IMAGE=${GCC_LATEST_IMAGE}
    GCC_TAG=gcc-${OPT_TAG}
    CLANG_COMPILER_IMAGE=${CLANG_LATEST_IMAGE}
    CLANG_TAG=clang-${OPT_TAG}
    ;;
  "NONE")
    GCC_TAG=${OPT_GCC_IMAGE}
    CLANG_TAG=${OPT_CLANG_IMAGE}
    ;;
  *)
    GCC_TAG=gcc-${OPT_TAG}
    CLANG_TAG=clang-${OPT_TAG}
    ;;
esac

if ${OPT_DRY}; then
  dry="echo"
else
  dry=""
fi

if ${OPT_PUSH}; then
  push="--push"
else
  push=""
  echo "warning: images are not pushed."
fi

cd ${SCRIPTDIR}
if test "${GCC_COMPILER_IMAGE}" != "NONE"; then
  ${dry} docker build --build-arg "COMPILER_IMAGE=${GCC_COMPILER_IMAGE}" \
         --target target-image -t ${REPOSITORY}:${GCC_TAG} .
else
  echo "no image built for GCC compiler"
fi

if test "${CLANG_COMPILER_IMAGE}" != "NONE"; then
  ${dry} docker build --build-arg "COMPILER_IMAGE=${CLANG_COMPILER_IMAGE}" \
        --target target-image -t ${REPOSITORY}:${CLANG_TAG} .
else
  echo "no image built for CLANG compiler"
fi

