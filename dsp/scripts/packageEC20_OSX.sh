#!/bin/bash

result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
fi

if [ -d "../deployment/EmissionControl20_OSX" ]; then
  rm -rf ../deployment/EmissionControl20_OSX
fi

chmod 644 bin/libraries/libsndfile/*
cp -r bin ../deployment
mv ../deployment/bin ../deployment/EmissionControl20_OSX
cd ../deployment/EmissionControl20_OSX
mkdir -p EmissionControl20.app/Contents/MacOS
mkdir EmissionControl20.app/Contents/Resources
mv runPackage_OSX.sh EmissionControl20.app/Contents/MacOS/
mv * EmissionControl20.app/Contents/Resources
chmod +x EmissionControl20.app/Contents/MacOS/runPackage_OSX.sh
chmod 444 ../../dsp/bin/libraries/libsndfile/*
# chmod 444 libraries/libsndfile/*