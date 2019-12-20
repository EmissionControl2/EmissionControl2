cmake_minimum_required(VERSION 3.0)

# name of application. replace 'app' with desired app name
set(APP_NAME emissionControl)
project(${APP_NAME} VERSION 0.5
                    DESCRIPTION "Granular Scheduler for Arbitrary Sound Files"
                    LANGUAGES CXX)

add_subdirectory(external/allolib)
add_subdirectory(external/libsamplerate)
add_subdirectory(external/al_ext)

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

add_executable(${APP_NAME}
  src/main.cpp
)

target_include_directories(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/libsamplerate/src/)
target_include_directories(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external)
target_include_directories(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_directories(${APP_NAME} PUBLIC /usr/local/lib)

message(${CMAKE_CURRENT_SOURCE_DIR}/external/allolib/external/Gamma/)

add_library(ecLib
  STATIC 
    ${includes}
    ${srcs}
)

target_include_directories(ecLib PRIVATE ${CMAKE_SOURCE_DIR}/external)
target_include_directories(ecLib PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # On macOS if jack is present, RtMidi will link to jack but not provide the
    # /usr/local/lib location as link directory. This statement only serves the
    # purpose of working around that bug. Perhaps can be removed on future
    # updates of RtMidi
    link_directories("/usr/local/lib")
    link_directories("/Library/Frameworks/Jackmp.framework/Versions/A/")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

get_target_property(AL_EXT_LIBRARIES al_ext AL_EXT_LIBRARIES)

target_link_libraries(ecLib PUBLIC al "${AL_EXT_LIBRARIES}" ${CMAKE_SOURCE_DIR}/external/libsamplerate/build/libsamplerate.a)

target_link_libraries(${APP_NAME} PUBLIC ecLib)


set_target_properties(ecLib PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
)

# binaries are put into the ./bin directory by default
set_target_properties(${APP_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
 	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/buildTest)

