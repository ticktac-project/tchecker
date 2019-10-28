#!/usr/bin/env bash

if test ! -n "${TCK_EXAMPLES_DIR}";
then
    echo 1>&2 "missing environment variable TCK_EXAMPLES_DIR"
    exit 1
fi

if test -n "${TCHECKER}";
then
    exec ${TCHECKER} explore -m zg:elapsed:extraLUg ${TCK_EXAMPLES_DIR}/ad94.txt
else
    echo 1>&2 "missing environment variable TCHECKER"
    exit 1
fi

