#!/usr/bin/env bash

if test -n "${TCK_REACH}";
then
    exec ${TCK_REACH} -a covreach < /dev/null
else
    echo 1>&2 "missing environment variable TCK_REACH"
    exit 1
fi

