cmake_minimum_required(VERSION 3.0)

# name of application. replace 'app' with desired app name
set(APP_NAME emissionControl)
project(${APP_NAME} VERSION 0.5
                    DESCRIPTION "Granular Scheduler for Arbitrary Sound Files"
                    LANGUAGES CXX)

add_executable(${APP_NAME} 
  src/main.cpp
)
add_subdirectory(external/allolib)
#add_subdirectory(external/libsamplerate)
add_subdirectory(external/al_ext)

set(includes
  include/emissionControl.h
  include/const.h
  include/utility.h
)

set(srcs
  src/emissionControl.cpp
  src/ecInterface.cpp
  src/granular.cpp
  src/utility.cpp
)

target_include_directories(${APP_NAME} PRIVATE
  ${includes}
  ${srcs}
)

target_include_directories(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/libsamplerate/src/)
#target_include_directories(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/allolib/external/Gamma/Gamma/)

add_library(ecLib
  STATIC 
    ${includes}
    ${srcs}
)

target_link_libraries(${APP_NAME} PRIVATE al)
target_link_libraries(${APP_NAME} PRIVATE ecLib)
get_target_property(AL_EXT_LIBRARIES al_ext AL_EXT_LIBRARIES)
target_link_libraries(${APP_NAME} PRIVATE ${AL_EXT_LIBRARIES})



# binaries are put into the ./bin directory by default
set_target_properties(${APP_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CXX_FLAGS "-Woverloaded-virtual" 
  CXX_FLAGS "-Wdeprecated-declarations"
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
 	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/buildTest)

