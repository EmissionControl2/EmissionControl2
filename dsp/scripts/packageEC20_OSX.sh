#!/bin/bash

result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
fi

if [ -d "../deployment/EmissionControl20_OSX" ]; then
  rm -rf ../deployment/EmissionControl20_OSX
fi

cp -r bin ../deployment
mv ../deployment/bin ../deployment/EmissionControl20_OSX