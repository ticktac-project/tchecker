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

# Model

echo "# A model is a list of declarations. Objects must be declared before they are 
# used. For instance, the source and target locations of an edge must be
# declared before the edge. Please, notice that all declarations are in the 
# global scope. The model must start with a system declaration.
# 
# system:name
# clock:size:name
# int:size:min:max:init:name
# process:name
# event:name
# location:process:name
# edge:process:source:target:event
# sync:events
#   where events is a colon-separated list of process@event, for instance:
#   P1@a:P2@b:P3@c
#
# Each declaration can be appended a list of attributes between braces, for 
# instance: { initial: : invariant: x<1 }
# Each attribute is a pair "key: value" and attributes are separated by 
# columns. It is syntactically valid to omit the values (as for
# attribute "initial" above), but some keys may require a value (for instance,
# attribute "invariant").
#
# Please, refer to TChecker wiki for a detailed documentation of the input 
# language
"

echo "system:parallel_${N}
"

# Events

echo "event:tau
event:s
"

# Processes

for pid in `seq 1 $N`; do
    echo "# Process $pid
process:P$pid
clock:1:x$pid
location:P$pid:A{initial:}	
location:P$pid:B{}
location:P$pid:C{}
edge:P$pid:A:B:tau{do:x$pid=0}
edge:P$pid:B:C:s{provided:x$pid<=1}
"
done

# Synchronizations

echo -n "sync";
for pid in `seq 1 $N`; do
    echo -n ":P$pid@s";
done;
echo ""
