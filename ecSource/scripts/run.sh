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
   
   Example: './scripts/run.sh -j5' (for a 4-core processor)
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
result=$?
if [ ${result} == 0 ]; then

  cd bin

  if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ./EmissionControl2
  elif [[ "$OSTYPE" == "darwin"* ]]; then
    TERM=${2:-DEFAULT}
    chmod 444 EmissionControl2.app/Contents/Resources/libsndfile/*
    if [ ${TERM} == "-g" ]; then
      ./EmissionControl2.app/Contents/MacOS/EmissionControl2
    else
      open EmissionControl2.app
    fi
  elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    ./EmissionControl2.exe
  fi
fi
