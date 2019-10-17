#!/usr/bin/env bash

MODEL=zg:elapsed:NOextra
if test -n "${TCHECKER}";
then
    exec ${TCHECKER} covreach -c aMl -m ${MODEL} -S <<EOF
system:S
process:P
location:P:A{initial:}
event:tau
edge:P:A:A:tau{}
EOF

else
    echo 1>&2 "missing environment variable TCHECKER"
    exit 1
fi
