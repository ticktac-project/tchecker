#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Generates a TChecker model for the dining philosophers, with a timeout
# to release the left fork if the right fork cannot be obtained in time,
# in order to avoid deadlocks. Inspired from:
# D. Lugiez, P. Niebert and S. Zennou, "A partial order semantics approach
# to the clock explosion problem of timed automata", TCS 2005

# Checks command line arguments
if [ $# -eq 1 ];
then
    N=$1        # number of philosophers
    TIMEOUT=3   # time-out to acquire the second fork
    EAT=10      # time required by a philosopher to eat
    SLOW=0      # time in-between releasing the left fork and the right fork
else
    if [ $# -ge 4 ];
    then
	N=$1        # number of philosophers
	TIMEOUT=$2  # time-out to acquire the second fork
	EAT=$3      # time required by a philosopher to eat
	SLOW=$4     # time in-between releasing the left fork and the right fork
    else
	echo "Usage: $0 <# philosophers>";
	echo "       $0 <# philosophers> <timeout> <eat> <slow>";
	echo " ";
	echo "where:   <# philosophers>  is the number of philosophers";
	echo "         <timeout>         is the timeout to release the left fork";
	echo "         <eat>             is the duration of the dinner";
	echo "         <slow>            is the delay to release forks";
	echo " ";
	echo "By default, we choose timeout=3, eat=10, slow=0";
	exit 1
    fi
fi

if [ "$N" -lt 2 ]; then
    echo "ERROR: number of philosophers should be >= 2";
    exit 1;
fi

if [ "$TIMEOUT" -lt 0 ]; then
    echo "ERROR: timeout should be >= 0"
    exit 1;
fi

if [ "$EAT" -lt 0 ]; then
    echo "ERROR: duration of dinner should be >= 0"
    exit 1;
fi

if [ "$SLOW" -lt 0 ]; then
    echo "ERROR: slowness should be >= 0"
    exit 1;
fi

# Labels
labels="eating1"
for pid in `seq 2 $N`; do
    labels="${labels},eating${pid}"
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

echo "system:dining_philosophers_${N}_${TIMEOUT}_${EAT}_${SLOW}
"

# Events

echo "# events
event:tau"

for p in `seq 1 $N`; do
    echo "event:take${p}
event:release${p}"
done
echo ""


# Processes

## Philosophers

for p in `seq 1 $N`; do
    if [ "$p" -eq 1 ]; then
	LEFT=$N
    else
	LEFT=$(($p-1))
    fi;
    RIGHT=$p
    echo "# Philosopher $p
process:P${p}
clock:1:x${p}
location:P${p}:idle{initial:}
location:P${p}:acq{invariant: x${p}<=${TIMEOUT}}
location:P${p}:eat{invariant: x${p}<=${EAT} : labels: eating${p}}
location:P${p}:rel{invariant: x${p}<=${SLOW}}
edge:P${p}:idle:acq:take${LEFT}{do: x${p}=0}
edge:P${p}:acq:idle:release${LEFT}{provided: x${p}>=${TIMEOUT}}
edge:P${p}:acq:eat:take${RIGHT}{provided: x${p}<=${TIMEOUT} : do: x${p}=0}
edge:P${p}:eat:rel:release${RIGHT}{provided: x${p}>=${EAT} : do: x${p}=0}
edge:P${p}:rel:idle:release${LEFT}
"
done


## Forks

for f in `seq 1 $N`; do
    echo "# Fork $f
process:F${f}
location:F${f}:free{initial:}
location:F${f}:taken
edge:F${f}:free:taken:take${f}
edge:F${f}:taken:free:release${f}
"
done

# Synchronizations

echo "# Synchronizations"

for p in `seq 1 $N`; do
    if [ "$p" -eq 1 ]; then
	LEFT=$N
    else
	LEFT=$(($p-1))
    fi;
    RIGHT=$p
    echo "sync:P${p}@take${LEFT}:F${LEFT}@take${LEFT}
sync:P${p}@take${RIGHT}:F${RIGHT}@take${RIGHT}
sync:P${p}@release${LEFT}:F${LEFT}@release${LEFT}
sync:P${p}@release${RIGHT}:F${RIGHT}@release${RIGHT}"
done
echo ""
