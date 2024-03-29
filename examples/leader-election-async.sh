#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Generates a TChecker model for a Leader election protocol over a ring inspired
# from "Fully Symbolic Timed Model Checking using Constraint Matrix Diagrams"
# R. Ehlers, D. Fass, M. Gerke and H-J Peter, RTSS 2010.
# Model without global variable, compatible with asynchronous zone graph

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

echo "system:leader_election_async_${NPROCS}_${TIMEOUT}
"

# Events

echo "# Events
event:tau
event:read_out"

for p in `seq 1 $NPROCS`; do
    echo "event:finished${p}
event:lock${p}
event:unlock${p}
event:read_out_prev${p}"
done
echo ""

# Global variables

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
int:1:1:${NPROCS}:${p}:in${p}
int:1:1:${NPROCS}:${p}:out${p}
location:C${p}:eval{invariant: x${p}<=1}
location:C${p}:init{initial: : invariant: x${p}<=0}
location:C${p}:leader
location:C${p}:read{invariant: x${p}<=1}
location:C${p}:release{invariant: x${p}<=1}
location:C${p}:wait_prev{invariant: x${p}<=0}
location:C${p}:wait_succ
location:C${p}:wait_succ_read_out
edge:C${p}:eval:eval:read_out
edge:C${p}:init:init:read_out
edge:C${p}:leader:leader:read_out
edge:C${p}:read:read:read_out
edge:C${p}:release:release:read_out
edge:C${p}:wait_prev:wait_prev:read_out
edge:C${p}:wait_succ:wait_succ:read_out
edge:C${p}:wait_succ_read_out:wait_succ_read_out:read_out
edge:C${p}:eval:leader:finished${p}{provided: in${p}==${p}}
edge:C${p}:eval:wait_succ:tau{provided: in${p}!=${p} : do: out${p}=in${p}}
edge:C${p}:init:read:lock${PREV}{do: x${p}=0}
edge:C${p}:init:wait_succ_read_out:lock${p}
edge:C${p}:read:release:read_out_prev${p}{do: in${p}=out${PREV}}
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
sync:C${p}@finished${p}:S@finished${p}
sync:C${p}@read_out_prev${p}:C${PREV}@read_out"
done
echo ""
