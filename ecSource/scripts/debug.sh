#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControl2" ]; then
  cd ecSource/
fi


(
  if [ $(uname -s) == "Linux" ]; then
    cmake --build ./build/debug -j 5
  fi
  if [ $(uname -s) == "Darwin" ]; then
    chmod 644 bin/EmissionControl2.app/Contents/Resources/libsndfile/*
    cmake --build ./build/debug -j 5
  fi
)
result=$?
if [ ${result} == 0 ]; then
  cd bin

  if [ $(uname -s) == "Linux" ]; then
    gdb -ex run ./EmissionControl2 -DRTAUDIO_API_JACK=1 -DRTMIDI_API_JACK=0
  fi

  DEBUG=${1:-DEFAULT}
  if [ $(uname -s) == "Darwin" ]; then
      chmod 444 ./EmissionControl2.app/Contents/Resources/libsndfile/*
      lldb ./EmissionControl2.app/Contents/MacOS/EmissionControl2
  fi

fi