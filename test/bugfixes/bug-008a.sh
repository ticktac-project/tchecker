#!/usr/bin/env bash

if test -n "${TCK_REACH_SH}";
then
  inputfile=tmp$$.tck
  cat > ${inputfile} <<EOF
system:S
process:P
location:P:A{initial:}
event:tau
edge:P:A:A:tau{}
EOF
  ${TCK_REACH_SH} -a covreach ${inputfile}
  res=$?
  rm -f ${inputfile}
  exit ${res}
else
  echo 1>&2 "missing environment variable TCK_REACH_SH"
  exit 1
fi
