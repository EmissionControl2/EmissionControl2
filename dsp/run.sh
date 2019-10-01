#!/bin/bash
(
  # utilizing cmake's parallel build options
  # recommended: -j <number of processor cores + 1>
  cmake --build ./build/release -j 5
)

result=$?
if [ ${result} == 0 ]; then
   cd bin
  ./emissionControl
fi
