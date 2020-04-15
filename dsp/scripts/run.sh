#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
fi
(
  # utilizing cmake's parallel build options
  # recommended: -j <number of processor cores + 1>
  cmake --build ./build/release -j 5
  if [ $(uname -s) == "Linux" ]; then
    chmod 644 bin/Resources/libsndfile/* #allow permissions to wewrite load commands of exectuables
    #rewrite load commands for dynamic libraries
  fi
  if [ $(uname -s) == "Darwin" ]; then
    cp -r ../externalResources/Fonts bin/emissionControl20.app/Contents/Resources
    chmod 644 bin/emissionControl20.app/Contents/Resources/libsndfile/*
    install_name_tool -change /usr/local/opt/libsndfile/lib/libsndfile.1.dylib @rpath/libsndfile/libsndfile.1.dylib bin/emissionControl20.app/Contents/MacOS/emissionControl20
    install_name_tool -change /usr/local/opt/flac/lib/libFLAC.8.dylib @rpath/libsndfile/libFLAC.8.dylib bin/emissionControl20.app/Contents/Resources/libsndfile/libsndfile.1.dylib
    install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib @rpath/libsndfile/libogg.0.dylib bin/emissionControl20.app/Contents/Resources/libsndfile/libFLAC.8.dylib
    install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib @rpath/libsndfile/libogg.0.dylib bin/emissionControl20.app/Contents/Resources/libsndfile/libsndfile.1.dylib
    install_name_tool -change /usr/local/opt/libvorbis/lib/libvorbis.0.dylib @rpath/libsndfile/libvorbis.0.dylib bin/emissionControl20.app/Contents/Resources/libsndfile/libsndfile.1.dylib
    install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib @rpath/libsndfile/libogg.0.dylib bin/emissionControl20.app/Contents/Resources/libsndfile/libvorbis.0.dylib
    install_name_tool -change /usr/local/opt/libvorbis/lib/libvorbisenc.2.dylib @rpath/libsndfile/libvorbisenc.2.dylib bin/emissionControl20.app/Contents/Resources/libsndfile/libsndfile.1.dylib
    install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib @rpath/libsndfile/libogg.0.dylib bin/emissionControl20.app/Contents/Resources/libsndfile/libvorbisenc.2.dylib
    install_name_tool -change /usr/local/Cellar/libvorbis/1.3.6/lib/libvorbis.0.dylib @rpath/libsndfile/libvorbis.0.dylib bin/emissionControl20.app/Contents/Resources/libsndfile/libvorbisenc.2.dylib
    chmod 444 bin/emissionControl20.app/Contents/Resources/libsndfile/*
  fi
)

result=$?
# pwd
if [ ${result} == 0 ]; then

  if [ $(uname -s) == "Linux" ]; then
    cd bin
    ./EmissionControl20 -DRTAUDIO_API_JACK=1 -DRTMIDI_API_JACK=1
  fi

  if [ $(uname -s) == "Darwin" ]; then
    cd bin
    open EmissionControl20.app
  fi

fi
