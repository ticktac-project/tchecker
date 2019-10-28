#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Check parameters

K=10

function usage() {
    echo "Usage: $0 N";
    echo "       N number of processes";
}

if [ $# -eq 1 ]; then
    N=$1
else
    usage
    exit 1
fi

# Labels
labels="access1"
for pid in `seq 2 $N`; do
    labels="${labels},access${pid}"
done
echo "#labels=${labels}"

# Model

echo "#clock:size:name
#int:size:min:max:init:name
#process:name
#event:name
#location:process:name{attributes}
#edge:process:source:target:event:{attributes}
#sync:events
#   where
#   attributes is a colon-separated list of key:value
#   events is a colon-separated list of process@event
"

echo "system:parallel_bis${N}
"

# Events

echo "event:tau
event:acquire
event:release
"

# Processes

for pid in `seq 1 $N`; do
    echo "# Process ${pid}
process:P${pid}
clock:1:x${pid}
location:P${pid}:A{initial:}	
location:P${pid}:B{}
location:P${pid}:C{invariant:x${pid}<=3 : labels: access${pid}}
edge:P${pid}:A:B:tau{do:x${pid}=0}
edge:P${pid}:B:A:tau{provided:x${pid}>=1}
edge:P${pid}:B:C:acquire{provided:x${pid}<1 : do:x${pid}=0}
edge:P${pid}:C:A:release{provided:x${pid}>=1}
"
done

echo "# Process lock
process:lock
clock:1:y
location:lock:U{initial:}
location:lock:L{}
edge:lock:U:L:acquire{provided:y>=1}
edge:lock:L:U:release{do:y=0}
"

for pid in `seq 1 $N`; do
echo "sync:P${pid}@acquire:lock@acquire
sync:P${pid}@release:lock@release
"
done