cmake_minimum_required(VERSION 3.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING "Minimum OS X deployment version")


# name of application. replace 'app' with desired app name
set(APP_NAME EmissionControl2)
project(${APP_NAME} VERSION 1.2
                    DESCRIPTION "Granular Scheduler for Arbitrary Sound Files"
                    LANGUAGES CXX)

add_subdirectory(external/allolib)
add_subdirectory(external/libsamplerate)
add_subdirectory(external/al_ext)


if (NOT TARGET al_soundfile)
    message(FATAL_ERROR "al_soundfile not built. Likely libsndfile not found")
endif()

# Hacky (and fragile way) to move libsndfile-1.dll to bin/Resources
if(WIN32)
    get_filename_component(sndfile_dir ${SNDFILE_LIBRARY} DIRECTORY)
    set(${sndfile_dir}/../bin/libsndfile-1.dll)
    message("WINDOWS: Attempting to copy sndfile dll to bin/")
    file(COPY ${sndfile_dir}/../bin/libsndfile-1.dll
            DESTINATION ${CMAKE_SOURCE_DIR}/bin)
endif()
# OSX library needed for native file dialog.

if(APPLE)
  find_library(AppKit Appkit)
endif()

# Linux library needed for native file dialog.
if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
  include_directories(${GTK3_INCLUDE_DIRS})
  link_directories(${GTK3_LIBRARY_DIRS})
endif()

# Windows library needed for native file dialog. **PUT HERE**

set(includes
  include/emissionControl.h
  include/ecInterface.h
  include/ecSynth.h
  include/utility.h
  include/const.h
)

set(srcs
  src/emissionControl.cpp
  src/ecInterface.cpp
  src/ecSynth.cpp
  src/utility.cpp
)
if(APPLE)
  set(RESOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/EmissionControl2.app/Contents/Resources")
  set(CMAKE_MACOSX_RPATH 1)

  set(MACOSX_BUNDLE_ICON_FILE "EmissionControl2.icns")
  set(ICON ${CMAKE_CURRENT_SOURCE_DIR}/../externalResources/osx/EmissionControl2.icns)
  set_source_files_properties(${ICON} PROPERTIES
       MACOSX_PACKAGE_LOCATION "Resources")

  set(CMAKE_INSTALL_RPATH "@executable_path/../Resources/")
  set(RESOURCE_LIST
      "libsndfile"
      "fonts"
      "samples"
  )

  foreach(RESOURCE_DIR_NAME ${RESOURCE_LIST})
    SET(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../externalResources/${RESOURCE_DIR_NAME}")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      file(GLOB CHILDREN RELATIVE "${RESOURCE_DIR}" ${RESOURCE_DIR}/*)
      foreach(CHILD ${CHILDREN})
        IF(IS_DIRECTORY "${RESOURCE_DIR}/${CHILD}")
          # be wary of .DS_Store files. glob them out by using [A-Za-z]*.*
          FILE(GLOB RESOURCE_DIR_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${RESOURCE_DIR}/${CHILD}/[A-Za-z]*.*")
          SET_SOURCE_FILES_PROPERTIES(
            ${RESOURCE_DIR_FILES}
            PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${RESOURCE_DIR_NAME}/${CHILD}
          )
          SET(RESOURCE_FILES ${RESOURCE_FILES} "../externalResources/${RESOURCE_DIR_FILES}")
        ELSEIF(NOT ${CHILD} MATCHES "\\.DS_Store|.*\\.am")
          SET_SOURCE_FILES_PROPERTIES(
            "${RESOURCE_DIR}/${CHILD}"
            PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${RESOURCE_DIR_NAME}
          )
          SET(RESOURCE_FILES ${RESOURCE_FILES} "../externalResources/${RESOURCE_DIR_NAME}/${CHILD}")
        ENDIF()
      endforeach()
    else(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") #WORK ON
      INSTALL( DIRECTORY ${RESOURCE_DIR} DESTINATION "${DATA_DIR}" PATTERN "*.am" EXCLUDE)
    endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  endforeach()
endif(APPLE)

if(WIN32)
  set(ICON ../externalResources/windows/ec2-windows.rc)
endif(WIN32)

add_executable(${APP_NAME} MACOSX_BUNDLE
  src/main.cpp
  ${RESOURCE_FILES}
  ${ICON}
)

target_include_directories(${APP_NAME} PRIVATE 
  ${CMAKE_SOURCE_DIR}/external
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/external/nativefiledialog/src/include
  ${CMAKE_SOURCE_DIR}/external/libsamplerate/src/
)


# Link native file dialog.
# target_link_directories(${APP_NAME} PUBLIC 
#   /usr/local/lib
#   ${CMAKE_SOURCE_DIR}/external/nativefiledialog/build/lib/release/x64
# )

find_library(NFD_LIBRARY nfd HINTS 
  ${CMAKE_SOURCE_DIR}/external/nativefiledialog/build/lib/Release/x64
)

add_library(ecLib
  STATIC 
    ${includes}
    ${srcs}
)

target_include_directories(ecLib PRIVATE 
  ${CMAKE_SOURCE_DIR}/external
  ${CMAKE_SOURCE_DIR}/include
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # On macOS if jack is present, RtMidi will link to jack but not provide the
    # /usr/local/lib location as link directory. This statement only serves the
    # purpose of working around that bug. Perhaps can be removed on future
    # updates of RtMidi
    link_directories("/usr/local/lib")
    link_directories("/Library/Frameworks/Jackmp.framework/Versions/A/")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

get_target_property(AL_EXT_LIBRARIES al_ext AL_EXT_LIBRARIES)

target_link_libraries(ecLib PUBLIC 
  al 
  "${AL_EXT_LIBRARIES}"
  samplerate
  ${NFD_LIBRARY}
) 

if(APPLE)
  target_link_libraries(ecLib PUBLIC ${Appkit})
endif()

if(LINUX)
  target_link_libraries(ecLib PUBLIC ${GTK3_LIBRARIES})
endif()

# Link windows libraries.
if(WIN32)
  target_link_libraries(ecLib PUBLIC comctl32)
endif()

target_link_libraries(${APP_NAME} PUBLIC ecLib ${LIBRARIES})


set_target_properties(ecLib PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
)

# binaries are put into the ./bin directory by default
set_target_properties(${APP_NAME} PROPERTIES
  BUILD_WITH_INSTALL_RPATH 1
  INSTALL_NAME_DIR "@rpath"
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin
)

## POST BUILD COMMANDS TO LINK DYNAMIC LIBRARIES
if(APPLE)
  set_target_properties(${APP_NAME} PROPERTIES
  MACOSX_BUNDLE_BUNDLE_NAME "${APP_NAME}"
  MACOSX_BUNDLE_GUI_IDENTIFIER "EmissionControl2"
  MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../externalResources/osx/MacOSXBundleInfo.plist.in
  )
  add_custom_command (TARGET ${APP_NAME}
    POST_BUILD 
    COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
      "-change" "/usr/local/opt/libsndfile/lib/libsndfile.1.dylib" 
      "@rpath/libsndfile/libsndfile.1.dylib"
      "$<TARGET_FILE:${APP_NAME}>" 

    COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
      "-change" "/usr/local/opt/libvorbis/lib/libvorbis.0.dylib" 
      "@rpath/libsndfile/libvorbis.0.dylib"
      "${RESOURCE_PATH}/libsndfile/libsndfile.1.dylib"
    COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
      "-change" "/usr/local/opt/flac/lib/libFLAC.8.dylib" 
      "@rpath/libsndfile/libFLAC.8.dylib"
      "${RESOURCE_PATH}/libsndfile/libsndfile.1.dylib"
    COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
      "-change" "/usr/local/opt/libogg/lib/libogg.0.dylib" 
      "@rpath/libsndfile/libogg.0.dylib"
      "${RESOURCE_PATH}/libsndfile/libsndfile.1.dylib"
    COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
      "-change" "/usr/local/opt/libvorbis/lib/libvorbisenc.2.dylib" 
      "@rpath/libsndfile/libvorbisenc.2.dylib"
      "${RESOURCE_PATH}/libsndfile/libsndfile.1.dylib" 

    COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
    "-change" "/usr/local/opt/libogg/lib/libogg.0.dylib" 
    "@rpath/libsndfile/libogg.0.dylib"
    "${RESOURCE_PATH}/libsndfile/libFLAC.8.dylib"

    COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
      "-change" "/usr/local/opt/libogg/lib/libogg.0.dylib" 
      "@rpath/libsndfile/libogg.0.dylib"
      "${RESOURCE_PATH}/libsndfile/libvorbis.0.dylib"

    COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
      "-change" "/usr/local/opt/libogg/lib/libogg.0.dylib" 
      "@rpath/libsndfile/libogg.0.dylib"
      "${RESOURCE_PATH}/libsndfile/libvorbisenc.2.dylib" 
    COMMAND "${CMAKE_INSTALL_NAME_TOOL}"
      "-change" "/usr/local/Cellar/libvorbis/1.3.6/lib/libvorbis.0.dylib" 
      "@rpath/libsndfile/libvorbis.0.dylib"
      "${RESOURCE_PATH}/libsndfile/libvorbisenc.2.dylib" 
    VERBATIM
  )
  
endif(APPLE)
