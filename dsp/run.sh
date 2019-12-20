#!/bin/bash
(
  # utilizing cmake's parallel build options
  # recommended: -j <number of processor cores + 1>
  cmake --build ./build/release -j 5 
  install_name_tool -change /usr/local/opt/libsndfile/lib/libsndfile.1.dylib @rpath/libsndfile/libsndfile.1.dylib bin/emissionControl
  install_name_tool -change usr/local/opt/flac/lib/libFLAC.8.dylib @rpath/libsndfile/libFLAC.8.dylib bin/emissionControl
)

result=$?
if [ ${result} == 0 ]; then
   cd bin
  ./emissionControl -DRTAUDIO_API_JACK=0 -DRTMIDI_API_JACK=0 
fi
