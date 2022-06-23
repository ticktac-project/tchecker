#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Check parameters

K=10

function usage() {
    echo "Usage: $0 N";
    echo "       $0 N K";
    echo "       N number of processes";
    echo "       K delay for mutex (default: $K)"
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
    labels="${labels}:cs${pid}"
done
echo "#labels=${labels}"

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

echo "system:fischer_${N}_$K
"

# Events

echo "event:tau
"

# Global variables

echo "int:1:0:$N:0:id
"

# Processes

for pid in `seq 1 $N`; do
    echo "# Process $pid
process:P$pid
clock:1:x$pid
location:P$pid:A{initial:}	
location:P$pid:req{invariant:x$pid<=$K}
location:P$pid:wait{}
location:P$pid:cs{labels:cs$pid}
edge:P$pid:A:req:tau{provided:id==0 : do:x$pid=0}
edge:P$pid:req:wait:tau{provided:x$pid<=$K : do:x$pid=0;id=$pid}
edge:P$pid:wait:req:tau{provided:id==0 : do:x$pid=0}
edge:P$pid:wait:cs:tau{provided:x$pid>$K&&id==$pid}
edge:P$pid:cs:A:tau{do:id=0}
"
done
