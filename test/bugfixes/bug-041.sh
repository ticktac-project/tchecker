#!/usr/bin/env bash

set -eu

INPUTFILE=${SRCDIR}/fischer_2.txt

echo "COVREAH output"
${TCK_REACH_SH} -a covreach ${INPUTFILE}
echo "EXPLORE output"
${TCK_REACH_SH} -a reach -s bfs ${INPUTFILE}

