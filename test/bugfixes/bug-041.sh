#!/usr/bin/env bash

set -eu

MODEL=zg:elapsed:NOextra
INPUTFILE=${SRCDIR}/fischer_2.txt

echo "COVREAH output"
${TCHECKER} covreach -l foo -m ${MODEL} -f dot ${INPUTFILE}
echo "EXPLORE output"
${TCHECKER} explore -s bfs -m ${MODEL} -f dot ${INPUTFILE}

