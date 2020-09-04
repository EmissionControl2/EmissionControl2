#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControlPort" ]; then
  cd dsp/
fi
(
  if [ $(uname -s) == "Linux" ]; then
    cmake --build ./build/release -j 5
  fi
  if [ $(uname -s) == "Darwin" ]; then
    chmod 644 bin/EmissionControl2.app/Contents/Resources/libsndfile/*
    cmake --build ./build/release -j 5
  fi
)
result=$?
if [ ${result} == 0 ]; then

  if [ $(uname -s) == "Linux" ]; then
    cd bin
    ./EmissionControl2 -DRTAUDIO_API_JACK=1 -DRTMIDI_API_JACK=0
  fi

  DEBUG=${1:-DEFAULT}
  if [ $(uname -s) == "Darwin" ]; then
      chmod 444 bin/EmissionControl2.app/Contents/Resources/libsndfile/*
    cd bin
    if [ ${DEBUG} == "-g" ]; then
      ./EmissionControl2.app/Contents/MacOS/EmissionControl2
    else
      open EmissionControl2.app
    fi
  fi
fi
