#!/usr/bin/env bash

if test -n "${TCHECKER}";
then
    exec ${TCHECKER} covreach < /dev/null
else
    echo 1>&2 "missing environment variable TCHECKER"
    exit 1
fi

