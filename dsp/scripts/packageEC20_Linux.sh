#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControlPort" ]; then
  cd dsp/
fi

if [ -d "../deployment/EmissionControl20_Linux" ]; then
  rm -rf ../deployment/EmissionControl20_Linux
fi

chmod 644 bin/Resources/libsndfile/*
cp -r bin ../deployment/Linux
mv ../deployment/Linux/bin ../deployment/Linux/EmissionControl20_Linux
chmod 444 bin/Resources/libsndfile/*
chmod 444 ../deployment/Linux/EmissionControl20_Linux/Resources/libsndfile/*
