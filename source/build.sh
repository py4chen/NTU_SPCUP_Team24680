#!/bin/bash
file="recorder"
aubio="../../aubio" # path of aubio
gcc -Wall -l asound -L $aubio/build/src -l aubio -I $aubio/src $file.c -o $file
