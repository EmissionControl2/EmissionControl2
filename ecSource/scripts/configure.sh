#!/bin/bash

# if needed to create debug or other builds replace 'release' in the following
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControl2" ]; then
  cd ecSource/
fi

# Make sure submodules exist.
(
cd ..
git submodule update --init --recursive
)

rm -f external/al_ext/assets3d/CMakeLists.txt
rm -f external/al_ext/openvr/CMakeLists.txt
rm -f external/al_ext/spatialaudio/CMakeLists.txt
rm -f external/al_ext/statedistribution/CMakeLists.txt

(
if [ $(uname -s) == "Linux" ]; then
  mkdir -p ./bin
  cd ./bin
  mkdir -p ./Resources
  cp -r ../../externalResources/Fonts ./Resources
  cp -r ../../externalResources/samples ./Resources
fi
)

# Build LIBSAMPLERATE if it doesnt exist../external/libsamplerate/build
(
if [ ! -d "./external/libsamplerate/build" ]; then
  cmake -E make_directory external/libsamplerate/build
  cd external/libsamplerate/build
  cmake ..
  make
fi
)

# Build nativefiledialog if it doesnt exist../external/libsamplerate/build
(
if [ ! -d "./external/nativefiledialog/build/lib" ]; then
  cd external/nativefiledialog/build/
  if [ $(uname -s) == "Linux" ]; then
    cd gmake_linux
    make config=release_x64
  elif [ $(uname -s) == "Darwin" ]; then # note: can't get make file to work, relies on xcode bleh
    cd xcode4
    xcodebuild -scheme nfd build -project nfd.xcodeproj/ -configuration Release CFLAGS=-mmacosx-version-min=10.10 CXXFLAGS=-mmacosx-version-min=10.10
  fi
fi
)


(
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

(
mkdir -p build
cd build
mkdir -p debug
cd debug
if [ $(uname -s) == "Linux" ]; then
  cmake -DCMAKE_BUILD_TYPE=Debug -Wno-deprecated -DBUILD_EXAMPLES=0 -DRTAUDIO_API_JACK=1 -DRTMIDI_API_JACK=1 ../..
fi

if [ $(uname -s) == "Darwin" ]; then
  cmake -DCMAKE_BUILD_TYPE=Debug -Wno-deprecated -DBUILD_EXAMPLES=0 -DRTAUDIO_API_JACK=0 -DRTMIDI_API_JACK=0 ../..
fi
)
