#!/bin/bash

# if needed to create debug or other builds replace 'release' in the following
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControlPort" ]; then
  cd dsp/
fi
(

  if [ $(uname -s) == "Linux" ]; then
    mkdir -p ./bin
    cd ./bin
    mkdir -p ./Resources
    cp -r ../../externalResources/Fonts ./Resources
    mkdir -p Resources/config_scripts
    cp -p ../../externalResources/config_scripts/configAbsoDirectories-linux.sh ./Resources/config_scripts/
    cd ..
  fi

  # Build LIBSAMPLERATE if it doesnt exist../external/libsamplerate/build
  if [ ! -d "./external/libsamplerate/build" ]; then
    cmake -E make_directory external/libsamplerate/build
    cd external/libsamplerate/build
    cmake ..
    make
    cd ../../../
  fi

  # Build nativefiledialog if it doesnt exist../external/libsamplerate/build
  if [ ! -d "./external/nativefiledialog/build/lib" ]; then
    cd external/nativefiledialog/build/
    if [ $(uname -s) == "Linux" ]; then
      cd gmake_linux
      make config=release_x64
    elif [ $(uname -s) == "Darwin" ]; then
      cd gmake_macosx
      make config=release_x64
    fi
    cd ../../../../
  fi

  mkdir -p build
  cd build
  mkdir -p release
  cd release
  if [ $(uname -s) == "Linux" ]; then
    cmake -DCMAKE_BUILD_TYPE=Release -Wno-deprecated -DBUILD_EXAMPLES=0 -DRTAUDIO_API_JACK=1 -DRTMIDI_API_JACK=1 ../..
  fi

  if [ $(uname -s) == "Darwin" ]; then
    cmake -DCMAKE_BUILD_TYPE=Release -Wno-deprecated -DBUILD_EXAMPLES=0 -DRTAUDIO_API_JACK=0 -DRTMIDI_API_JACK=0 ../..
  fi

)
