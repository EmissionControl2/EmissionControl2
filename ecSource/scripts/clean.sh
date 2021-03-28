#!/bin/bash

# if needed to create debug or other builds replace 'release' in the following
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControl2" ]; then
  cd ecSource/
fi
(
 rm -r -f ./external/libsamplerate/build/
 rm -r -f ./external/nativefiledialog/build/lib/
 rm -r -f ./external/nativefiledialog/build/gmake_macosx/obj
 rm -r -f ./external/nativefiledialog/build/gmake_linux/obj
 rm -r -f ./bin/ 
 rm -r -f ./build/
)