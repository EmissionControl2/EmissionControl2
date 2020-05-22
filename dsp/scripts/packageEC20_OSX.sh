#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControlPort" ]; then
  cd dsp/
fi

if [ -d "../deployment/OSX/EmissionControl20" ]; then
  rm -rf ../deployment/OSX/EmissionControl20
fi

if [ -f "../deployment/OSX/EmissionControl20.pkg" ]; then
  rm -rf ../deployment/OSX/EmissionControl20.pkg
fi
cd bin
sudo pkgbuild --install-location /Applications --component EmissionControl20.app/ ../../deployment/OSX/EmissionControl20.pkg 