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
