#!/bin/sh

CRAX=/home/ais3/crax
QEMU=/home/ais3/crax/build/qemu-release/i386-s2e-softmmu/qemu-system-i386
if [ -z $1 ]; then
echo "Usage: start.sh image"
else
$QEMU -hda $S2EDIR/img/$1 -s2e-config-file $CRAX/s2e/config.lua -loadvm demo -s2e-verbose -net none
fi
