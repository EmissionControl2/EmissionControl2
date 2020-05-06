#!/bin/bash

# if needed to create debug or other builds replace 'release' in the following
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControlPort" ]; then
  cd dsp/
fi
(
#  rm -r -f ./external/libsamplerate/build/
 rm -r -f ./bin/ 
 rm -r -f ./build/
)