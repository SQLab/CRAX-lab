#!/bin/sh

QEMU=/home/ais3/crax/build/qemu-release/i386-s2e-softmmu/qemu-system-i386
if [ -z $1 ]; then
echo "Usage: raw.sh image [snapshot]"
elif [ -z $2 ]; then
$QEMU -hda $S2EDIR/img/$1 -net none -monitor stdio
else
$QEMU -hda $S2EDIR/img/$1 -loadvm $2 -net none -monitor stdio
fi
