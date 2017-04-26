#!/bin/bash

echo "gcc -static sample.c -o sample"
gcc -static sample.c -o sample
echo "s2ecmd fork 0"
s2ecmd fork 0
echo "./sample $(python -c 'print "a"*0x1000')"
./sample $(python -c 'print "a"*0x1000')


