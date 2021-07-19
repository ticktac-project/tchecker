#!/usr/bin/env bash

if test ! -n "${TCK_EXAMPLES_DIR}";
then
    echo 1>&2 "missing environment variable TCK_EXAMPLES_DIR"
    exit 1
fi

if test -n "${TCK_REACH_SH}";
then
    exec ${TCK_REACH_SH} -a reach -C /dev/stdout ${TCK_EXAMPLES_DIR}/ad94.txt
else
    echo 1>&2 "missing environment variable TCK_REACH"
    exit 1
fi

