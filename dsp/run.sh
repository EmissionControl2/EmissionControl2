#!/bin/bash
(
  # utilizing cmake's parallel build options
  # recommended: -j <number of processor cores + 1>
  cmake --build ./build/release -j 5 

  chmod 644 bin/libraries/libsndfile/* #allow permissions to wewrite load commands of exectuables
  #rewrite load commands for dynamic libraries
  install_name_tool -change /usr/local/opt/libsndfile/lib/libsndfile.1.dylib @rpath/libsndfile/libsndfile.1.dylib bin/emissionControl
      install_name_tool -change /usr/local/opt/flac/lib/libFLAC.8.dylib @rpath/libsndfile/libFLAC.8.dylib bin/libraries/libsndfile/libsndfile.1.dylib
          install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib @rpath/libsndfile/libogg.0.dylib bin/libraries/libsndfile/libFLAC.8.dylib
      install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib @rpath/libsndfile/libogg.0.dylib bin/libraries/libsndfile/libsndfile.1.dylib
      install_name_tool -change /usr/local/opt/libvorbis/lib/libvorbis.0.dylib @rpath/libsndfile/libvorbis.0.dylib bin/libraries/libsndfile/libsndfile.1.dylib
          install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib @rpath/libsndfile/libogg.0.dylib bin/libraries/libsndfile/libvorbis.0.dylib
    install_name_tool -change /usr/local/opt/libvorbis/lib/libvorbisenc.2.dylib @rpath/libsndfile/libvorbisenc.2.dylib bin/libraries/libsndfile/libsndfile.1.dylib
        install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib @rpath/libsndfile/libogg.0.dylib bin/libraries/libsndfile/libvorbisenc.2.dylib
        install_name_tool -change /usr/local/Cellar/libvorbis/1.3.6/lib/libvorbis.0.dylib @rpath/libsndfile/libvorbis.0.dylib bin/libraries/libsndfile/libvorbisenc.2.dylib
  chmod 444 bin/libraries/libsndfile/*
)

result=$?
if [ ${result} == 0 ]; then
   cd bin
  ./emissionControl -DRTAUDIO_API_JACK=0 -DRTMIDI_API_JACK=0 
fi
