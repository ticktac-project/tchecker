#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Generates a TChecker model for a Leader election protocol over a ring inspired
# from "Fully Symbolic Timed Model Checking using Constraint Matrix Diagrams"
# R. Ehlers, D. Fass, M. Gerke and H-J Peter, RTSS 2010.

### Checks command line arguments
if [ ! $# -eq 2 ];
then
    echo "Usage: $0 <# processes> <timeout>"
    exit 0;
fi

NPROCS=$1
TIMEOUT=$2 # 4 in RTSS10 paper

if [ $NPROCS -le 1 ]; # Number of processes at least 2
then
    echo "*** Invalid number of processes: $N";
    echo "The number of processes must be at least 2";
    exit 0;
fi

# Labels
echo "#labels=error"

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

echo "system:leader_election_${NPROCS}_${TIMEOUT}
"

# Events

echo "# Events
event:tau"

for p in `seq 1 $NPROCS`; do
    echo "event:finished${p}
event:lock${p}
event:unlock${p}"
done
echo ""

# Global variables

for p in `seq 1 $NPROCS`; do
    echo "int:1:1:${NPROCS}:${p}:in${p}
int:1:1:${NPROCS}:${p}:out${p}"
done
echo ""

# Processes

## Candidate process

for p in `seq 1 $NPROCS`; do
    if [ "$p" -eq 1 ]; then
	PREV=$NPROCS
    else
	PREV=$(($p-1))
    fi
    
    echo "# Candidate process $p
process:C${p}
clock:1:x${p}
location:C${p}:eval{invariant: x${p}<=1}
location:C${p}:init{initial: : invariant: x${p}<=0}
location:C${p}:leader
location:C${p}:read{invariant: x${p}<=1}
location:C${p}:release{invariant: x${p}<=1}
location:C${p}:wait_prev{invariant: x${p}<=0}
location:C${p}:wait_succ
location:C${p}:wait_succ_read_out
edge:C${p}:eval:leader:finished${p}{provided: in${p}==${p}}
edge:C${p}:eval:wait_succ:tau{provided: in${p}!=${p} : do: out${p}=in${p}}
edge:C${p}:init:read:lock${PREV}{do: x${p}=0}
edge:C${p}:init:wait_succ_read_out:lock${p}
edge:C${p}:read:release:tau{do: in${p}=out${PREV}}
edge:C${p}:release:eval:unlock${PREV}
edge:C${p}:wait_prev:read:lock${PREV}{do: x${p}=0}
edge:C${p}:wait_succ:wait_succ_read_out:lock${p}
edge:C${p}:wait_succ_read_out:wait_prev:unlock${p}{do: x${p}=0}
"
done

## Stopwatch process

echo "# Stopwatch process
process:S
clock:1:z
location:S:error{labels: error}
location:S:init{initial:}
location:S:safe
edge:S:init:error:tau{provided: z>$TIMEOUT}"

for p in `seq 1 $NPROCS`; do
    echo "edge:S:init:safe:finished${p}{provided: z<=$TIMEOUT}"
done
echo ""

# Synchronizations

echo "# Synchronizations"

for p in `seq 1 $NPROCS`; do
        if [ "$p" -eq 1 ]; then
	PREV=$NPROCS
    else
	PREV=$(($p-1))
    fi
    echo "sync:C${p}@lock${PREV}:C${PREV}@lock${PREV}
sync:C${p}@unlock${PREV}:C${PREV}@unlock${PREV}
sync:C${p}@finished${p}:S@finished${p}"
done
echo ""
