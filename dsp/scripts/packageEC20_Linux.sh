#!/bin/bash

result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
fi

if [ -d "../packages/EmissionControl20_Linux" ]; then
  rm -rf ../packages/EmissionControl20_Linux
fi

chmod 644 bin/libraries/libsndfile/*
cp -r bin ../packages
mv ../packages/bin ../packages/EmissionControl20_Linux
chmod 444 bin/libraries/libsndfile/*
chmod 444 ../packages/EmissionControl20_Linux/libraries/libsndfile/*
