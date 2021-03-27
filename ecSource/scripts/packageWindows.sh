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
rm ./bin/EmissionControl2.ilk
rm ./bin/EmissionControl2.pdb
cp ../../LICENSE ./bin/
mv bin EmissionControl2
echo "EmissionControl2 moved to: $(pwd)"