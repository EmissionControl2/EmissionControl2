#!/bin/bash

# if needed to create debug or other builds replace 'release' in the following
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
fi
(
 rm -r -f ./external/libsamplerate/build/
 rm -r -f ./bin/ 
 rm -r -f ./build/
)

