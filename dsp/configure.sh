#!/bin/bash

# if needed to create debug or other builds replace 'release' in the following
(
  mkdir -p build
  cd build
  mkdir -p release
  cd release
  cmake -DCMAKE_BUILD_TYPE=Release -Wno-deprecated -DBUILD_EXAMPLES=0 ../..
)

