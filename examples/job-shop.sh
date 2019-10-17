#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Generates a TChecker model of a job shop scheduling problem as described in:
# "Scheduling with Timed Automata", Abdeddaïm, Asarin and Maler, TCS, 2004
# and:
# "Lower and Upper Bounds in Zone Based Abstractions of Timed Automata",
# Behrmann, Bouyer, Larsen and Pelanek, STTT, 2006.
#
# In the last paper, the authors do not check that each tasks lasts exactly
# N t.u., but at least N t.u. As far as one is concerned with schedulability,
# this is equivalent (If more than N CPU t.u. are given to the task, then
# consider the extra t.u. as idle CPU t.u.). This allows to have no upper-bounds
# on clocks which entails a great reduction with ExtraLU+
#
# The scripts generates a model with #J jobs and #M machines. Each job consists
# in upto #M tasks. Each task uses one of the machine for a given duration.
# Both the machine and the duration are chosen randomly. The accepting state
# (labelled "green") is reachable iff there is a feasible schedule within
# MAKESPAN time units

# Checks command line arguments
if [ $# -lt 4 ] || [ $# -gt 5 ];
then
    echo 1>&2 "Usage: $0 <# machines> <# jobs> <job max duration> <makespan> [<seed>]";
    exit 1
fi

NM=$1
NJ=$2
DJ=$3
MAKESPAN=$4

if test "x$5" != "x"; then
    RANDOM=$5
fi

# Labels
echo "#labels=scheduled"

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

echo "system:job_shop_${NM}_${NJ}_${DJ}_${MAKESPAN}
"


# Events

echo "# Events
event:tau
event:count"

for m in `seq 1 $NM`; do
    echo "event:get${m}
event:rel${m}"
done

for j in `seq 1 $NJ`; do
    echo "event:done${j}"
done
echo ""

# Processes

## Machines
for m in `seq 1 $NM`; do
    echo "# Machine $m
process:M${m}
location:M${m}:idle{initial:}
location:M${m}:busy
edge:M${m}:idle:busy:get${m}
edge:M${m}:busy:idle:rel${m}
"
done

## Jobs
for j in `seq 1 $NJ`;do
    echo "# Job $j
process:J${j}
clock:1:x${j}"

    NT=$(($RANDOM % $NM))  # Choose random number of tasks for job $j
    if [ "$NT" -eq 0 ]; then NT=1; fi;

    echo "location:J${j}:i1{initial:}
location:J${j}:w1
location:J${j}:i$((NT+1)){committed:}
location:J${j}:done"

    if [ "$NT" -ge 2 ]; then
	for t in `seq 2 $NT`; do
	    echo "location:J${j}:i${t}
location:J${j}:w${t}"
	done
    fi

    for t in `seq 1 $NT`; do
	M=$(($RANDOM % $NM + 1))  # Randomly choose a machine for task $t
	D=$(($RANDOM % $DJ + 1))  # Randomly choose a duration for task $t
	echo "edge:J${j}:i${t}:w${t}:get${M}{do: x${j}=0}
edge:J${j}:w${t}:i$((t+1)):rel${M}{provided: x${j}>=$D}"
    done

    echo "edge:J${j}:i$((NT+1)):done:done${j}
"
done

## Controller

echo "# Controller
process:C
clock:1:z
int:1:0:${NJ}:0:n
location:C:idle{initial:}
location:C:done{labels: scheduled}
edge:C:idle:idle:count{do: n=n+1}
edge:C:idle:done:count{provided: n==$((NJ-1)) && (z <= ${MAKESPAN}) : do: n=n+1}
"

# Synchronizations

for j in `seq 1 $NJ`; do
    echo "sync:J${j}@done${j}:C@count" # Job with controller
    for m in `seq 1 $NM`; do
	echo "sync:J${j}@get${m}:M${m}@get${m}
sync:J${j}@rel${m}:M${m}@rel${m}"
    done
done

echo ""
