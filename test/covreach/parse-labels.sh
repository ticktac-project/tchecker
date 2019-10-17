#!/usr/bin/env bash


while test $# != 1;
do
    COMMAND="${COMMAND} \"$1\""
    shift
done

INPUTFILE="$1"
if test -f ${INPUFILE};
then
    LABELS=$(grep -e "^# *labels *= *\([a-zA-Z0-9_,]*\) *\$" ${INPUTFILE} | sed -e 's/^# *labels *= *//g')
    if test -n "${LABELS}";
    then
        COMMAND="${COMMAND} -l \"${LABELS}\""
    fi
    COMMAND="${COMMAND} \"${INPUTFILE}\""
else
    echo 1>&2 "missing input file '${INPUTFILE}'"
    exit 1
fi

eval ${COMMAND}
