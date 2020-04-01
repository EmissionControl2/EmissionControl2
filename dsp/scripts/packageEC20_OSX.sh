#!/bin/bash

result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
fi

if [ -d "../packages/EmissionControl20_OSX" ]; then
  rm -rf ../packages/EmissionControl20_OSX
fi

chmod 644 bin/libraries/libsndfile/*
cp -r bin ../packages
mv ../packages/bin ../packages/EmissionControl20_OSX
cd ../packages/EmissionControl20_OSX
mkdir -p EmissionControl20.app/Contents/MacOS
mv emissionControl EmissionControl20.app/Contents/MacOS/emissionControl
chmod +x EmissionControl20.app/Contents/MacOS/emissionControl
# mv libraries EmissionControl20.app/Contents/Resources
chmod 444 ../../dsp/bin/libraries/libsndfile/*
chmod 444 libraries/libsndfile/*