#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Variant of the classical model of Fischer's protocol where the shared
# variable id is modeled as a process (required from asynchrnous zone graph)

# Check parameters

K=10

function usage() {
    echo "Usage: $0 N [K]";
    echo "       N number of processes";
    echo "       K delay for mutex (default: ${K})"
}

if [ $# -eq 1 ]; then
    N=$1
elif [ $# -eq 2 ]; then
    N=$1
    K=$2
else
    usage
    exit 1
fi

# Labels
labels="cs1"
for pid in `seq 2 $N`; do
    labels="${labels},cs${pid}"
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

echo "system:fischer_async_${N}_${K}
"

# Events

for i in `seq 0 $N`; do
    echo "event:id_is_${i}
event:id_to_${i}"
done
echo "event:tau
"

# Processes

for pid in `seq 1 $N`; do
    echo "# Process ${pid}
process:P${pid}
clock:1:x${pid}
location:P${pid}:A{initial:}	
location:P${pid}:req{invariant:x${pid}<=${K}}
location:P${pid}:wait{}
location:P${pid}:cs{labels:cs${pid}}
edge:P${pid}:A:req:id_is_0{do:x${pid}=0}
edge:P${pid}:req:wait:id_to_${pid}{provided:x${pid}<=${K} : do:x${pid}=0}
edge:P${pid}:wait:req:id_is_0{do:x${pid}=0}
edge:P${pid}:wait:cs:id_is_${pid}{provided:x${pid}>${K}}
edge:P${pid}:cs:A:id_to_0{}
"
done

# Global variables

echo "# Process ID
process:ID
int:1:0:$N:0:id
location:ID:l{initial:}"
for i in `seq 0 $N`; do
    echo "edge:ID:l:l:id_is_${i}{provided: id==${i}}
edge:ID:l:l:id_to_${i}{do: id=${i}}"
done
echo ""

# Synchronizations

echo "# Synchronizations
"

for pid in `seq 1 $N`; do
    echo "sync:P${pid}@id_is_0:ID@id_is_0
sync:P${pid}@id_to_0:ID@id_to_0
sync:P${pid}@id_is_${pid}:ID@id_is_${pid}
sync:P${pid}@id_to_${pid}:ID@id_to_${pid}"
done
echo ""
