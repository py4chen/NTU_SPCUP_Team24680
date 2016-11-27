#!/bin/bash
file="recorder"
aubio="../../aubio" # path of aubio
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$aubio/build/src
gcc -Wall -l asound -L $aubio/build/src -l aubio -I $aubio/build/src/ -I $aubio/tests/ -I $aubio/src -lm $file.c -o $file
