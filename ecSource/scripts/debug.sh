#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControl2" ]; then
  cd ecSource/
fi

THREADS=$1
if [[ "$THREADS" == "" ]]; then
  echo "
   NOTE: you can append '-j#', replacing # with your CPU's number of cores +1, to compile faster.
   
   Example: './scripts/debug.sh -j5' (for a 4-core processor)
   "
fi

(
  if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    cmake --build ./build/debug --config Debug $THREADS
  elif [[ "$OSTYPE" == "darwin"* ]]; then
    chmod 644 bin/EmissionControl2.app/Contents/Resources/libsndfile/*
    cmake --build ./build/debug --config Debug $THREADS
  elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    cmake --build ./build/debug --config Debug $THREADS
  fi
)
result=$?
if [ ${result} == 0 ]; then
  cd bin

  if [ $(uname -s) == "Linux" ]; then
    gdb -ex run ./EmissionControl2
  elif [ $(uname -s) == "Darwin" ]; then
    chmod 444 ./EmissionControl2.app/Contents/Resources/libsndfile/*
    lldb ./EmissionControl2.app/Contents/MacOS/EmissionControl2
  elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    echo "RUN WINDOWS DEBUGGER TODO"
  fi

fi
