#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Critical region, inspired from:
# "Fully Symbolic Model Checking for Timed Automata", Georges Morbé,
# Florian Pigorsch and Christoph Scholl, CAV 2011

if [ $# -eq 1 ];
then
  NPROCS=$1            # Number of processes
  T=10
else if [ $# -eq 2 ];
then
  NPROCS=$1
  T=$2
else  
	echo "Usage: $0 <# stations>";
	echo "       $0 <# stations> <t>";
	echo " ";
	echo "where:    <# stations> is the number of stations in the network";
	echo "          <t>          is the limit on process";
	echo " ";
	echo "The values must satisfy: #stations >= 1";
	echo " ";
	echo "The values in 'Fully Symbolic Model Checking for Timed Automata' \
CAV, 2011, were t=10";
	exit 0
  fi
fi


if [ ! $NPROCS -ge 1 ];
then
    echo "*** The number of processes must be at least 1"
    exit 0
fi

# Labels
labels="error1"
for pid in `seq 2 $NPROCS`; do
    labels="${labels},error${pid}"
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

echo "system:critical_region_${NPROCS}_${T}
"

# Events
echo "# Events
event:tau"

for pid in `seq 1 $NPROCS`; do
    echo "event:enter${pid}
event:exit${pid}"
done
echo ""

# Global variables
echo "# Global variables
int:1:0:${NPROCS}:0:id
"

# Counter process
echo "# Counter
process:counter
location:counter:I{initial:}
location:counter:C{}
edge:counter:I:C:tau{provided: id==0 : do: id=1}
edge:counter:C:C:tau{provided: id<${NPROCS} : do: id=id+1}
edge:counter:C:C:tau{provided: id==${NPROCS} : do: id=1}
"

# Arbiter processes
for pid in `seq 1 $NPROCS`; do
    echo "# Arbiter ${pid}
process:arbiter${pid}
location:arbiter${pid}:req{initial:}
location:arbiter${pid}:ack{}
edge:arbiter${pid}:req:ack:enter${pid}{provided: id==${pid} : do: id=0}
edge:arbiter${pid}:ack:req:exit${pid}{do: id=${pid}}
"
done

# Production cell processes
for pid in `seq 1 $NPROCS`; do
    echo "# Prod cell ${pid}
process:prodcell${pid}
clock:1:x${pid}
location:prodcell${pid}:not_ready{initial:}
location:prodcell${pid}:testing{invariant: x${pid}<=$T}
location:prodcell${pid}:requesting{}
location:prodcell${pid}:critical{invariant: x${pid}<=$((2*T))}
location:prodcell${pid}:testing2{invariant: x${pid}<=$T}
location:prodcell${pid}:safe{labels: safe${pid}}
location:prodcell${pid}:error{labels: error${pid}}
edge:prodcell${pid}:not_ready:testing:tau{provided: x${pid}<=$((2*T)) : do: x${pid}=0}
edge:prodcell${pid}:testing:not_ready:tau{provided: x${pid}>=$T : do: x${pid}=0}
edge:prodcell${pid}:testing:requesting:tau{provided: x${pid}<=$((T-1))}
edge:prodcell${pid}:requesting:critical:enter${pid}{do: x${pid}=0}
edge:prodcell${pid}:critical:error:tau{provided: x${pid}>=$((2*T))}
edge:prodcell${pid}:critical:testing2:exit${pid}{provided: x${pid}<=$((T-1)) : do: x${pid}=0}
edge:prodcell${pid}:testing2:error:tau{provided: x${pid}>=$T}
edge:prodcell${pid}:testing2:safe:tau{provided: x${pid}<=$((T-1))}
"
done

# Synchronizations

echo "# Synchronizations"
for pid in `seq 1 $NPROCS`; do
    echo "sync:arbiter${pid}@enter${pid}:prodcell${pid}@enter${pid}
sync:arbiter${pid}@exit${pid}:prodcell${pid}@exit${pid}"
done

