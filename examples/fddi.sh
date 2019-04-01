#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Check parameters

function usage() {
    echo "Usage: $0 N";
    echo "       $0 N TTRT SA TD";
    echo "";
    echo "       N number of processes";
    echo "       TTRT target token rotation timer (default: N*50)";
    echo "       SA synchronous allocation (default: 20)";
    echo "       TD token delay (default: 0)"
}

if [ $# -eq 1 ]; then
    N=$1
    TTRT=$(($N*50))
    SA=20
    TD=0
elif [ $# -eq 4 ]; then
    N=$1
    TTRT=$2
    SA=$3
    TD=$4
else
    usage
    exit 1
fi

if [ $N -lt 1 ]; then
    echo "The number of processes must be at least 1";
    exit 0
fi

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

echo "
# Model of the FDDI protocol inspired from:
# The tool Kronos, C. Daws, A. Oliveiro, S. Tripakis and S. Yovine,
# Hybrid Systems III, 1996

system:fddi_${N}_${TTRT}_${SA}_${TD}
"

# Events

echo "event:tau
event:TT
event:RT"
for pid in `seq 1 $N`; do
    echo "event:TT${pid}"
    echo "event:RT${pid}"
done
echo ""

# Global variables

# Processes

for pid in `seq 1 $N`; do
    echo "# Process ${pid}
process:P${pid}
clock:1:trt${pid}
clock:1:xA${pid}
clock:1:xB${pid}
location:P${pid}:q0{initial:}
location:P${pid}:q1{invariant: trt${pid}<=${SA}}
location:P${pid}:q2{invariant: trt${pid}<=${SA}}
location:P${pid}:q3{invariant: xA${pid}<=$((${TTRT}+${SA}))}
location:P${pid}:q4{}
location:P${pid}:q5{invariant: trt${pid}<=${SA}}
location:P${pid}:q6{invariant: trt${pid}<=${SA}}
location:P${pid}:q7{invariant: xB${pid}<=$((${TTRT}+${SA}))}
edge:P${pid}:q0:q1:TT{provided: trt${pid}>=${TTRT} : do: trt${pid}=0;xB${pid}=0}
edge:P${pid}:q0:q2:TT{provided: trt${pid}<${TTRT} : do: trt${pid}=0;xB${pid}=0}
edge:P${pid}:q1:q4:RT{provided: trt${pid}==${SA}}
edge:P${pid}:q2:q3:tau{provided: trt${pid}==${SA}}
edge:P${pid}:q3:q4:RT{}
edge:P${pid}:q4:q5:TT{provided: trt${pid}>=${TTRT} : do: trt${pid}=0;xA${pid}=0}
edge:P${pid}:q4:q6:TT{provided: trt${pid}<${TTRT} : do: trt${pid}=0;xA${pid}=0}
edge:P${pid}:q5:q0:RT{provided: trt${pid}==${SA}}
edge:P${pid}:q6:q7:tau{provided: trt${pid}==${SA}}
edge:P${pid}:q7:q0:RT{}
"    
done


# Ring

echo "# Ring
process:R
clock:1:t
location:R:q1{initial: : invariant: t<=${TD}}
location:R:r1{}"

for pid in `seq 2 $N`; do
    echo "location:R:q${pid}{invariant: t<=${TD}}
location:R:r${pid}{}"
done

for pid in `seq 1 $(($N-1))`; do
    echo "edge:R:q${pid}:r${pid}:TT${pid}{provided: t==${TD}}
edge:R:r${pid}:q$(($pid+1)):RT${pid}{do: t=0}"
done

echo "edge:R:q${N}:r${N}:TT${N}{provided: t==${TD}}
edge:R:r${N}:q1:RT${N}{do: t=0}
"

# Synchros

echo "# Synchros"
for pid in `seq 1 $N`; do
    echo "sync:P${pid}@TT:R@TT${pid}
sync:P${pid}@RT:R@RT${pid}"
done
