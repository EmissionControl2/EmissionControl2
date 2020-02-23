#!/bin/bash

# if needed to create debug or other builds replace 'release' in the following
(
  mkdir -p bin 
  cd bin 
  cp -r ../../samples .
  cp -r ../../Fonts .
  mkdir -p soundOutput
  cp -r ../external/libraries .
	cd ..

  # Build LIBSAMPLERATE if it doesnt exist../external/libsamplerate/build
  if ! [-d ./external/libsamplerate/build]; then
    cmake -E make_directory external/libsamplerate/build
    cd external/libsamplerate/build
    cmake ..
    make
    cd ../../../
  fi

  mkdir -p build
  cd build
  mkdir -p release
  cd release
  cmake -DCMAKE_BUILD_TYPE=Release -Wno-deprecated -DBUILD_EXAMPLES=0 -DRTAUDIO_API_JACK=1 -DRTMIDI_API_JACK=1 ../..
)

