#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Generates a TChecker model for the GPS-MC example, inspired from:
# "Component-based Abstraction Refinement for Timed Controller Synthesis"
# Hans-Jörg Peter and Robert Mattmüller, RTSS 2009

# Checks command line arguments
if [ ! $# -eq 4 ];
then
    echo "Usage: $0 <# stations> <# sub stations> <processing time> <timeout>"
    exit 0;
fi

NSTATIONS=$1
NSUBSTATIONS=$2
PROCTIME=$3
TIMEOUT=$4

# Labels
echo "#labels=error"

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

echo "system:gps_mc_${NSTATIONS}_${NSUBSTATIONS}_${PROCTIME}_${TIMEOUT}
"


# Events

echo "# Events
event:tau"

for i in `seq 0 $NSTATIONS`; do
    echo "event:finished$i"
done

for j in `seq 1 $((NSUBSTATIONS+1))`; do
    echo "event:start$j"
done
echo ""

# Processes

for i in `seq 1 $NSTATIONS`; do
    echo "# Station $i
process:S${i}
location:S${i}:idle{initial:}
location:S${i}:start{committed:}
location:S${i}:processing{}
location:S${i}:done{committed:}
edge:S${i}:idle:start:finished$((i-1)){}
edge:S${i}:start:processing:start1{}
edge:S${i}:processing:done:start$((NSUBSTATIONS+1)){}
edge:S${i}:done:idle:finished${i}{}
"
done

#
for i in `seq 1 $NSTATIONS`; do
    for j in `seq 1 $NSUBSTATIONS`; do
	echo "# Substation $i $j
process:Sub${i}_${j}
clock:1:x${i}_${j}
location:Sub${i}_${j}:idle{initial:}
location:Sub${i}_${j}:processing{invariant: x${i}_${j}<=$PROCTIME}
edge:Sub${i}_${j}:idle:processing:start${j}{do: x${i}_${j}=0}
edge:Sub${i}_${j}:processing:idle:start$((j+1)){provided: x${i}_${j}<=$PROCTIME}
"
    done
done

#
echo "# Property
process:P
clock:1:z
location:P:idle{initial:}
location:P:error{labels: error}
location:P:finished
location:P:ok
edge:P:idle:finished:finished0{do: z=0}
edge:P:finished:error:tau{provided: z>$TIMEOUT}
edge:P:finished:ok:finished${NSTATIONS}{provided: z<=$TIMEOUT}
edge:P:ok:idle:tau{}
"

# Synchronizations

echo "# Synchronizations
sync:P@finished0:S1@finished0
sync:S${NSTATIONS}@finished${NSTATIONS}:P@finished${NSTATIONS}"

for i in `seq 1 $((NSTATIONS-1))`; do # between stations
    echo "sync:S${i}@finished${i}:S$((i+1))@finished${i}"
done

for i in `seq 1 $NSTATIONS`; do # between stations and substations
    echo "sync:S${i}@start1:Sub${i}_1@start1"
    echo "sync:S${i}@start$((NSUBSTATIONS+1)):Sub${i}_${NSUBSTATIONS}@start$((NSUBSTATIONS+1))"
done

for i in `seq 1 $NSTATIONS`; do # between substations
    for j in `seq 1 $((NSUBSTATIONS-1))`; do
	echo "sync:Sub${i}_${j}@start$((j+1)):Sub${i}_$((j+1))@start$((j+1))"
    done
done
echo ""
