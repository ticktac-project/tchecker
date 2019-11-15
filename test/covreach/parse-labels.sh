#!/usr/bin/env bash

if ! test -n "${DOT_MAX_SIZE}";
then
    DOT_MAX_SIZE=1000
fi

DOTFORMAT="no"
while test $# != 1;
do
    if test "$1" = "-f" -a "$2" = "dot";
    then
        DOTFORMAT="yes"
    fi
    COMMAND="${COMMAND} \"$1\""
    shift
done

TMPDOTFILE="tmpdotfile.$$.dot"
INPUTFILE="$1"
if test -f ${INPUFILE};
then
    LABELS=$(grep -e "^# *labels *= *\([a-zA-Z0-9_,]*\) *\$" ${INPUTFILE} | sed -e 's/^# *labels *= *//g')
    if test -n "${LABELS}";
    then
        COMMAND="${COMMAND} -l \"${LABELS}\""
    fi
    if test "${DOTFORMAT}" = "yes";
    then
        COMMAND="${COMMAND} -o "${TMPDOTFILE}""
    fi
    COMMAND="${COMMAND} \"${INPUTFILE}\""
else
    echo 1>&2 "missing input file '${INPUTFILE}'"
    exit 1
fi

eval ${COMMAND}

if test -f ${TMPDOTFILE};
then
    test $(cat ${TMPDOTFILE} | wc -l) -lt ${DOT_MAX_SIZE} && cat ${TMPDOTFILE}
    rm -f ${TMPDOTFILE}
fi


