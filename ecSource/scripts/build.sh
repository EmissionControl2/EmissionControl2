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
   
   Example: './scripts/build.sh -j5' (for a 4-core processor)
   "
fi

(
  if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    cmake --build ./build/release --config Release $THREADS
  elif [[ "$OSTYPE" == "darwin"* ]]; then
    chmod 644 bin/EmissionControl2.app/Contents/Resources/libsndfile/*
    cmake --build ./build/release --config Release $THREADS
  elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    cmake --build ./build/release --config Release $THREADS
  fi
)

if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    read
fi