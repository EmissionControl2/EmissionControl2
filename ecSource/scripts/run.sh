#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControl2" ]; then
  cd ecSource/
fi


(
  if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    cmake --build ./build/release -j 5
  elif [[ "$OSTYPE" == "darwin"* ]]; then
    chmod 644 bin/EmissionControl2.app/Contents/Resources/libsndfile/*
    cmake --build ./build/release -j 5
  elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    echo "BUILD WINDOWS TODO"
    cmake --build ./build/release -j 5
  fi
)
result=$?
if [ ${result} == 0 ]; then

  cd bin

  if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ./EmissionControl2 -DRTAUDIO_API_JACK=1 -DRTMIDI_API_JACK=0
  elif [[ "$OSTYPE" == "darwin"* ]]; then
    TERM=${1:-DEFAULT}
    chmod 444 EmissionControl2.app/Contents/Resources/libsndfile/*
    if [ ${TERM} == "-g" ]; then
      ./EmissionControl2.app/Contents/MacOS/EmissionControl2
    else
      open EmissionControl2.app
    fi
  elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    echo "RUN WINDOWS TODO"
    ./EmissionControl2.exe -DRTAUDIO_API_JACK=1 -DRTMIDI_API_JACK=0
  fi
fi
