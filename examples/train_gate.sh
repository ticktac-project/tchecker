#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Check parameters

function usage() {
    echo "Usage: $0 N";
    echo "       N number of trains";
}

if [ $# -eq 1 ]; then
    N=$1
else
    usage
    exit 1
fi

# Labels
labels="cross1"
for pid in `seq 2 $N`; do
    labels="${labels},cross${pid}"
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

echo "system:train_gate_$N
"

# Events

echo "event:tau
event:go
event:appr
event:leave
event:stop"

for pid in `seq 1 $N`; do
    echo "event:go$pid
event:appr$pid
event:leave$pid
event:stop$pid"
done

echo ""

# Gate process
echo "# Gate
process:Gate
int:$N:1:$N:1:buffer
int:1:0:$((N-1)):0:head
int:1:0:$N:0:length
location:Gate:Free{initial:}
location:Gate:Occ{}
location:Gate:Transient{committed:}"

for pid in `seq 1 $N`; do
    echo "edge:Gate:Free:Occ:go$pid{provided:length>0&&buffer[head]==$pid}
edge:Gate:Free:Occ:appr$pid{provided:length==0 : do:buffer[(head+length)%$N]=$pid;length=length+1}
edge:Gate:Occ:Transient:appr$pid{do:buffer[(head+length)%$N]=$pid;length=length+1}
edge:Gate:Occ:Free:leave$pid{provided:length>0&&buffer[head]==$pid : do:head=(head+1)%$N;length=length-1}
edge:Gate:Transient:Occ:stop$pid{provided:length>0&&buffer[(head+length-1)%$N]==$pid}"
done

echo ""

# Train processes

for pid in `seq 1 $N`; do
    echo "# Train $pid
process:Train$pid
clock:1:x$pid
location:Train$pid:Safe{initial:}
location:Train$pid:Appr{invariant:x$pid<=20}
location:Train$pid:Stop{}
location:Train$pid:Start{invariant:x$pid<=15}
location:Train$pid:Cross{invariant:x$pid<=5 : labels:cross$pid}
edge:Train$pid:Safe:Appr:appr{do:x$pid=0}
edge:Train$pid:Appr:Cross:tau{provided:x$pid>=10 : do:x$pid=0}
edge:Train$pid:Appr:Stop:stop{provided:x$pid<=10}
edge:Train$pid:Stop:Start:go{do:x$pid=0}
edge:Train$pid:Start:Cross:tau{provided:x$pid>=7 : do:x$pid=0}
edge:Train$pid:Cross:Safe:leave{provided:x$pid>=3}
sync:Train$pid@appr:Gate@appr$pid
sync:Train$pid@stop:Gate@stop$pid
sync:Train$pid@go:Gate@go$pid
sync:Train$pid@leave:Gate@leave$pid
"
done
