cmake_minimum_required(VERSION 3.0)

# name of application. replace 'app' with desired app name
set(APP_NAME emissionControl)

project(${APP_NAME})

link_directories("/usr/local/lib")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external")
# path to main source file
add_executable(${APP_NAME} src/main.cpp) ### ADD CPP FILES BELOW 

# add allolib as a subdirectory to the project
add_subdirectory(external/allolib)
add_subdirectory(external/al_ext)
#add_subdirectory(external/libsamplerate)

get_target_property(AL_EXT_LIBRARIES al_ext AL_EXT_LIBRARIES)

target_include_directories(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/libsamplerate/src/)
target_include_directories(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/allolib/external/Gamma/Gamma/)

# link allolib to project
target_link_libraries(${APP_NAME} PRIVATE  al ${AL_EXT_LIBRARIES})
target_link_libraries(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/libsamplerate/build/libsamplerate.a)
#target_link_libraries(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/emissionControl.cpp)


#include_directories(external/libsamplerate/src)

# example line for find_package usage
# find_package(Qt5Core REQUIRED CONFIG PATHS "C:/Qt/5.12.0/msvc2017_64/lib" NO_DEFAULT_PATH)

# replace ${PATH_TO_INCLUDE_DIR} before including other libraries
# target_include_directories(${APP_NAME} PRIVATE ${PATH_TO_INCLUDE_DIR})

# replace ${PATH_TO_LIB_FILE} before linking other libraries
# target_link_libraries(${APP_NAME} PRIVATE ${PATH_TO_LIB_FILE})

# binaries are put into the ./bin directory by default
set_target_properties(${APP_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
 	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin
)
cmake_minimum_required(VERSION 3.0)
project(granular)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/buildTest)



include_directories(include)

