# Install script for directory: /Users/jkilgore/Projects/EmissionControlPort/dsp/external/allolib/external

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/cmake_install.cmake")
  include("/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/cmake_install.cmake")
  include("/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/cmake_install.cmake")
  include("/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/cmake_install.cmake")
  include("/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/cmake_install.cmake")
  include("/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/cpptoml/cmake_install.cmake")
  include("/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/json/cmake_install.cmake")
  include("/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/stb/cmake_install.cmake")
  include("/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/dr_libs/cmake_install.cmake")

endif()

