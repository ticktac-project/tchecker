#!/usr/bin/env bash

if test $# != 1;
then
    echo 1>&2 "usage: $0 inputfile"
    exit 1
fi

INPUTFILE="$1"
TLINES=$(wc -l ${INPUTFILE} | awk '{print $1}')
N=1
while read line; do
    printf "%3d%%\r" $(expr 100 '*' ${N} '/' ${TLINES})
    src=$(echo ${line} | awk -F : '{ print $1 }')
    dst=$(echo ${line} | awk -F : '{ print $2 }')
    test -s ${src} && cp ${src} ${dst}
    let N=$N+1
done < ${INPUTFILE}

exit 0
