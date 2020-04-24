#!/bin/bash

result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
fi

if [ -d "../deployment/OSX/EmissionControl20" ]; then
  rm -rf ../deployment/OSX/EmissionControl20
fi

if [ -f "../deployment/OSX/EmissionControl20.dmg" ]; then
  rm -rf ../deployment/OSX/EmissionControl20.dmg
fi

cp -r bin ../deployment/OSX
cd ../deployment
mv OSX/bin OSX/EmissionControl20
hdiutil create -volname EmissionControl20 -srcfolder ../deployment/OSX -ov EmissionControl20.dmg
mv EmissionControl20.dmg OSX/
cd OSX/
echo "EmissionControl20.dmg moved to: $(pwd)"