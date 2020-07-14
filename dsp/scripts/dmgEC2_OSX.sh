#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControlPort" ]; then
  cd dsp/
fi

if [ -d "../deployment/OSX/EmissionControl2.app" ]; then
  rm -rf ../deployment/OSX/EmissionControl2.app
fi

if [ -f "../deployment/OSX/EmissionControl2.dmg" ]; then
  rm -rf ../deployment/OSX/EmissionControl2.dmg
fi

cp -r bin/EmissionControl2.app ../deployment/OSX
cd ../deployment
hdiutil create -volname EmissionControl2.app -srcfolder ../deployment/OSX -ov EmissionControl2.dmg
mv EmissionControl2.dmg OSX/
cd OSX/
echo "EmissionControl2.dmg moved to: $(pwd)"
