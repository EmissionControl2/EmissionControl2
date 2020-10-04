#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControl2" ]; then
  cd ecSource/
fi

rm -rf "../deployment/OSX/"
mkdir -p "../deployment/OSX/"

cp -r bin/EmissionControl2.app ../deployment/OSX
cd ../deployment
hdiutil create -volname EmissionControl2.app -srcfolder ../deployment/OSX -ov EmissionControl2.dmg
mv EmissionControl2.dmg OSX/
cd OSX/
cp ../../docs/EmissionControl2-Manual.pdf .
zip EmissionControl2-OSX.zip EmissionControl2.dmg EmissionControl2-manual.pdf
rm -rf EmissionControl2.app
rm EmissionControl2.dmg EmissionControl2-manual.pdf



echo "EmissionControl2.zip moved to: $(pwd)"
