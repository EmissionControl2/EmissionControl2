#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControlPort" ]; then
  cd dsp/
fi

if [ -d "../deployment/OSX/EmissionControl20.app" ]; then
  rm -rf ../deployment/OSX/EmissionControl20.app
fi

if [ -f "../deployment/OSX/EmissionControl20.dmg" ]; then
  rm -rf ../deployment/OSX/EmissionControl20.dmg
fi

cp -r bin/EmissionControl20.app ../deployment/OSX
cd ../deployment
hdiutil create -volname EmissionControl20.app -srcfolder ../deployment/OSX -ov EmissionControl20.dmg
mv EmissionControl20.dmg OSX/
cd OSX/
echo "EmissionControl20.dmg moved to: $(pwd)"