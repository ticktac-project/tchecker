#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Check parameters

L=808  # lambda
S=26   # sigma

function usage() {
    echo "Usage: $0 N";
    echo "       $0 N L S";
    echo "       N number of processes";
    echo "       L (lambda) delay for full communication";
    echo "       S (sigma) delay for collision detection";
}

if [ $# -eq 1 ]; then
    N=$1
elif [ $# -eq 3 ]; then
    N=$1
    L=$2
    S=$3
else
    usage
    exit 1
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

echo "system:csmacd_${N}_${L}_$S
"

# Events
echo "event:tau
event:begin
event:busy
event:end
event:cd"

for pid in `seq 1 $N`; do
    echo "event:cd$pid"
done

echo ""

# Bus process
echo "# Bus
process:Bus
int:1:1:$(($N+1)):1:j
clock:1:y
location:Bus:Idle{initial:}
location:Bus:Active{}
location:Bus:Collision{invariant:y<$S}
location:Bus:Loop{committed:}
edge:Bus:Idle:Active:begin{do:y=0}
edge:Bus:Active:Collision:begin{provided:y<$S : do:y=0}
edge:Bus:Active:Active:busy{provided:y>=$S}
edge:Bus:Active:Idle:end{do:y=0}
edge:Bus:Collision:Loop:tau{provided:y<$S : do:j=1}
edge:Bus:Loop:Idle:tau{provided:j==$N+1&&y<$S : do:y=0;j=1}"

for pid in `seq 1 $N`; do
    echo "edge:Bus:Loop:Loop:cd$pid{provided:j==$pid : do:j=j+1}"
done

echo ""

# Station processes
for pid in `seq 1 $N`; do
    echo "# Station $pid
process:Station$pid
clock:1:x$pid
location:Station$pid:Wait{initial:}
location:Station$pid:Start{invariant:x$pid<=$L}
location:Station$pid:Retry{invariant:x$pid<2*$S}
edge:Station$pid:Wait:Start:begin{do:x$pid=0}
edge:Station$pid:Wait:Retry:busy{do:x$pid=0}
edge:Station$pid:Wait:Wait:cd{do:x$pid=0}
edge:Station$pid:Wait:Retry:cd{do:x$pid=0}
edge:Station$pid:Start:Wait:end{provided:x$pid==$L : do:x$pid=0}
edge:Station$pid:Start:Retry:cd{provided:x$pid<$S : do:x$pid=0}
edge:Station$pid:Retry:Start:begin{provided:x$pid<2*$S : do:x$pid=0}
edge:Station$pid:Retry:Retry:busy{provided:x$pid<2*$S : do:x$pid=0}
edge:Station$pid:Retry:Retry:cd{provided:x$pid<2*$S : do:x$pid=0}
sync:Bus@begin:Station$pid@begin
sync:Bus@busy:Station$pid@busy
sync:Bus@cd$pid:Station$pid@cd
sync:Bus@end:Station$pid@end
"
done
