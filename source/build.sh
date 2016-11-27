#!/bin/bash
file="recorder"
aubio="../../aubio" # path of aubio
gcc $file.c -Wall -l asound -L $aubio/build/src -l aubio -I $aubio/src -o $file
