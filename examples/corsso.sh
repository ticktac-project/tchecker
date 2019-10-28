#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Generates a TChecker model for the Cornell Single Sifn-On (CorSSO)
# protocol inspired from:
# "SAT-based model-checking for region automata", Fang Yu and Bow-Yaw Wang,
# International Journal of Foundations of Computer Science, 17(4), 2006

# Checks command line arguments
if [ $# -eq 1 ];
then
    N=$1        # number of processes
    TA=2        # min time to acquire a certificate
    TE=10       # max time to acquire all certificates
    LTH="1 2"   # numbers of required certificates for each policy
    NTH=2       # size of TH (seen as an array)
else
    if [ $# -ge 4 ];
    then
	N=$1        # number of processes
	TA=$2       # min time to acquire a certificate
	TE=$3       # max time to acquire all certificates
	shift
	shift
	shift
	LTH=$*      # number of required certificates for each policy
	NTH=$#      # size of TH (seen as an array)
    else
	echo "Usage: $0 <# processes>";
	echo "       $0 <# processes> <TA> <TE> <TH1> ... <THn>";
	echo " ";
	echo "where:   <# processes>  is the number of processes";
	echo "         <TA>           is the min time to acquire a certificate";
	echo "         <TE>           is the max time to acquire a certificate";
	echo "         <THi>          is the number of certificates for policy i";
	echo " ";
	echo "By default, we choose TA=2, TE=10, TH=1 2";
	exit 1
    fi
fi

# Computes the maximum number of required certificates
MAXTH=0
for n in $LTH; do
    if [ "$n" -gt "$MAXTH" ];
    then
	let MAXTH=$n
    fi
done

declare -a TH
let i=0
for th in $LTH; do
    TH[i]=$th
    i=$(($i+1))
done

# Labels
labels="access1"
for pid in `seq 2 $N`; do
    labels="${labels},access${pid}"
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

echo "system:CorSSO_${N}_${TA}_${TE}_`echo ${LTH} | tr \" \" \"_\"`
"

# Events

echo "event:tau
"

# Processes
for pid in `seq 1 $N`; do
    echo "# Process ${pid}
process:P${pid}
clock:1:x${pid}
clock:1:y${pid}
int:1:0:$((MAXTH+1)):0:a${pid}
int:1:0:$((MAXTH+1)):0:p${pid}
location:P${pid}:auth{initial:}
location:P${pid}:access{labels: access${pid}}
edge:P${pid}:auth:auth:tau{provided: p${pid}>0 && x${pid}>${TA} && a${pid}<${MAXTH} : do: a${pid}=a${pid}+1; x${pid}=0}"
    for j in `seq 0 $((NTH-1))`; do
	echo "edge:P${pid}:auth:auth:tau{provided: p${pid}==0 : do: p${pid}=$((j+1)); a${pid}=0; x${pid}=0; y${pid}=0}
edge:P${pid}:auth:access:tau{provided: y${pid}<${TE} && p${pid}==$((j+1)) && a${pid} >= ${TH[$j]}}"
    done
    echo "edge:P${pid}:access:auth:tau{do: p${pid}=0}
"
done				   
