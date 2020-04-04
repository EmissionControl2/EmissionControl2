#!/bin/bash

result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
fi

if [ -d "../deployment/EmissionControl20_Linux" ]; then
  rm -rf ../deployment/EmissionControl20_Linux
fi

chmod 644 bin/Resources/libsndfile/*
cp -r bin ../deployment
mv ../deployment/bin ../deployment/EmissionControl20_Linux
chmod 444 bin/Resources/libsndfile/*
chmod 444 ../deployment/EmissionControl20_Linux/Resources/libsndfile/*
