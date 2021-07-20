#!/usr/bin/env bash

# This script is a wrapper that extract labels from TChecker files. It looks for
# a line # labels=l1:l2:... and then invokes tck-reach with the option
# -l l1,l2,...
# Additionally it filters the run time out line in order to make outputs usable
# in non-regression tests.
#

if ! test -n "${TCK_REACH}";
then
    echo 1>&2 "missing variable TCK_REACH"
    exit 1
fi

if ! test -n "${DOT_MAX_SIZE}";
then
    DOT_MAX_SIZE=1000
fi

COMMAND="${TCK_REACH}"
while test $# != 1;
do
    COMMAND="${COMMAND} \"$1\""
    shift
done

TMPDOTFILE="tmpdotfile.$$.dot"
INPUTFILE="$1"
if test -f ${INPUFILE};
then
    LABELS=$(grep -e "^# *labels *= *\([a-zA-Z0-9_:]*\) *\$" ${INPUTFILE} | sed -e 's/^# *labels *= *//g' | tr : ,)
    if test -n "${LABELS}";
    then
        COMMAND="${COMMAND} -l \"${LABELS}\""
    fi
    COMMAND="${COMMAND} -C \"${TMPDOTFILE}\" \"${INPUTFILE}\""
else
    echo 1>&2 "missing input file '${INPUTFILE}'"
    exit 1
fi

eval ${COMMAND} | sed -e 's/\(^RUNNING_TIME_SECONDS \).*$/\1 xxxx/g' -e 's@^@// @g'

if test -f ${TMPDOTFILE};
then
  if test $(cat ${TMPDOTFILE} | wc -l) -lt ${DOT_MAX_SIZE}; then
    cat ${TMPDOTFILE}
  else
    echo "// this dot file has been truncated to the first ${DOT_MAX_SIZE} lines"
    head -${DOT_MAX_SIZE} ${TMPDOTFILE}
  fi
  rm -f ${TMPDOTFILE}
fi


