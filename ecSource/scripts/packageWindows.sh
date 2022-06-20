#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControl2" ]; then
  cd ecSource/
fi

rm -rf "../deployment/Windows/"
mkdir -p "../deployment/Windows/"


cp -r bin ../deployment/Windows
cd ../deployment/Windows
cp ../../docs/EmissionControl2-Manual.pdf ./bin/
cp ../../LICENSE ./bin/
cp ../../externalResources/windows/README.txt ./bin/
mv bin EmissionControl2
echo "EmissionControl2 moved to: $(pwd)"