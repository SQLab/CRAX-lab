#!/bin/bash

echo "s2ecmd fork 0"
s2ecmd fork 0
echo "~/wrapper/symarg ./stage2 `python -c "print 'a'*0x1000"`"
~/wrapper/symarg ./stage2 `python -c "print 'a'*0x1000"`

