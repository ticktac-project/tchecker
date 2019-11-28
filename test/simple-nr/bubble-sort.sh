#!/usr/bin/env bash
#
# This file has been generated automatically with uppaal-to-tchecker
# 

if ! test -n "${TCHECKER}";
then
    echo 1>&2 "missing environment variable TCHECKER"
    exit 1
fi

SEED=12345
MAX=1000
N=10

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

exec ${TCHECKER} explore -f raw -m ta << EOF
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
edge:P:A:B:sortA{do: while (i < n - 1) do j = 0; while (j < n-i-1) do if (A[j] > A[j+1]) then tmp = A[j]; A[j] = A[j+1]; A[j+1] = tmp; tmp = 0 endif ; j = j + 1 done; i = i + 1 done; i = 0 }
edge:P:B:B:checkA{provided: i < n-1 : do : if A[i] <= A[i+1] then i = i+1 else error = 1 endif }
edge:P:B:ERR:checkA{provided: error }
EOF

