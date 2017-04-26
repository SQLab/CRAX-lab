#!/bin/bash

echo "gcc -static sample2.c -o sample2"
gcc -static sample2.c -o sample2
echo "s2ecmd fork 0"
s2ecmd fork 0
echo "~/wrapper/symarg ./sample2 `python -c \"print 'a'*0x1000\"`"
~/wrapper/symarg ./sample2 `python -c "print 'a'*0x1000"`


