#!/bin/sh

QEMU=/home/ais3/crax/build/qemu-release/i386-softmmu/qemu-system-i386
if [ -z $1 ]; then
echo "Usage: s2e.sh image [snapshot]"
elif [ -z $2 ]; then
$QEMU -hda $S2EDIR/img/$1 -net none
else
$QEMU -hda $S2EDIR/img/$1 -loadvm $2 -net none
fi
