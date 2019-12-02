#!/usr/bin/env bash
#
# This file has been generated automatically with uppaal-to-tchecker
# 
SEED=12345
MAX=1000
N=10
ML_DO=no
S_AND=no

usage() {
    cat >&2 << EOF
usage: $0 [-s seed] [-n array-size] [-m max-value] [--multilines-do] [--sequential-and] [-h]
Current values are: -s ${SEED} -n ${N} -m ${MAX}
Use -h for this help message.
EOF
}

TEMP=$(getopt -o "s:m:n:h" -l "sequential-and,multilines-do" -- "$@")
if test $? != 0; then
    usage $0
    exit 1
fi

eval set -- "${TEMP}"

while true ; do
        shifted=1
        case "$1" in
                -s) SEED=$2; shifted=2 ;;
                -n) N=$2; shifted=2 ;;
                -m) MAX=$2; shifted=2 ;;
                -h) usage $0; exit 0;;
                --multilines-do) ML_DO=yes ;;
                --sequential-and) S_AND=yes ;;
                --) shift; break ;;
                *) echo "Internal error on argument '$1' !" ; exit 1 ;;
        esac
        shift ${shifted}
done

if test $# != 0; then
    echo "wrong # arguments" >&2
    usage $0
    exit 1
fi

init_array() {
    RANDOM=$1
    printf "A[0]=$((RANDOM%MAX))"
    i=1
    while test $i -lt $N; do
        printf ";A[$i]=$((RANDOM%MAX))"
        let i=$i+1
    done
}

#clock:size:name
#int:size:min:max:init:name
#process:name
#event:name
#location:process:name{attributes}
#edge:process:source:target:event:{attributes}
#sync:events
#   where
#   attributes is a colon-separated list of key:value
#   events is a colon-separated list of process@event

cat << EOF
# File generated with the following setting:
# N = ${N} SEED = ${SEED} MAX = ${MAX}
# Multi-lines do = ${ML_DO}
# Sequential and = ${S_AND}

system: S
int:1:${N}:${N}:${N}:n
int:${N}:0:${MAX}:0:A
int:1:0:$((N+1)):0:i
int:1:0:$((N+1)):0:j
int:1:0:${MAX}:0:tmp
int:1:0:1:0:error

process:P
location:P:init{initial:}
location:P:A{}
location:P:B{}
location:P:ERR{}
event: initA
event: sortA
event: checkA
edge:P:init:A:initA{do:$(init_array ${SEED}) }
EOF

if test "${ML_DO}" = "yes"; then
    cat << EOF
edge:P:A:B:sortA{do:
while (i < n - 1) do
    j = 0;
    while (j < n-i-1) do
        if (A[j] > A[j+1]) then
            tmp = A[j];
            A[j] = A[j+1];
            A[j+1] = tmp;
            tmp = 0
        endif ;
        j = j + 1
     done;
     i = i + 1
done;
i = 0
}
EOF
    if  test "${S_AND}" = "yes"; then
        cat << EOF
edge:P:B:B:checkA{provided: i < n-1 && A[i] <= A[i+1] : do : i = i+1 }
edge:P:B:ERR:checkA{provided: i < n-1 && A[i] > A[i+1] }
EOF
    else
        cat << EOF
edge:P:B:B:checkA{provided: i < n-1 : do :
if A[i] <= A[i+1] then
    i = i+1
else
    error = 1
endif
}
edge:P:B:ERR:checkA{provided: error }
EOF
    fi
else
    cat << EOF
edge:P:A:B:sortA{do: while (i < n - 1) do j = 0; while (j < n-i-1) do if (A[j] > A[j+1]) then tmp = A[j]; A[j] = A[j+1]; A[j+1] = tmp; tmp = 0 endif ; j = j + 1 done; i = i + 1 done; i = 0 }
EOF
    if  test "${S_AND}" = "yes"; then
        cat << EOF
edge:P:B:B:checkA{provided: i < n-1 && A[i] <= A[i+1] : do : i = i+1 }
edge:P:B:ERR:checkA{provided: i < n-1 && A[i] > A[i+1] }
EOF
    else
        cat << EOF
edge:P:B:B:checkA{provided: i < n-1 : do : if A[i] <= A[i+1] then i = i+1 else error = 1 endif }
edge:P:B:ERR:checkA{provided: error }
EOF
    fi
fi

