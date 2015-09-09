#!/bin/sh

CRAX=/home/ais3/crax
QEMU=/home/ais3/crax/build/qemu-release/i386-s2e-softmmu/qemu-system-i386
if [ -z $1 ] || [ -z $2 ]; then
echo "Usage: raw.sh image snapshot"
else
$QEMU -s2e-config-file $CRAX/s2e/config.lua -hda $S2EDIR/img/$1 -loadvm $2 -s2e-verbose -net none
fi
