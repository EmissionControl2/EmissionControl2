cmake_minimum_required(VERSION 3.0)

# name of application. replace 'app' with desired app name
set(APP_NAME EmissionControl20)
project(${APP_NAME} VERSION 0.5
                    DESCRIPTION "Granular Scheduler for Arbitrary Sound Files"
                    LANGUAGES CXX)

add_subdirectory(external/allolib)
add_subdirectory(external/libsamplerate)
add_subdirectory(external/al_ext)

if(APPLE)
  find_library(AppKit Appkit)
endif()

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
  include_directories(${GTK3_INCLUDE_DIRS})
  link_directories(${GTK3_LIBRARY_DIRS})
endif()

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

set(RESOURCE_LIST
    "libsndfile"
)

foreach(RESOURCE_DIR_NAME ${RESOURCE_LIST})
  SET(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/Resources/${RESOURCE_DIR_NAME}")
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
        SET(RESOURCE_FILES ${RESOURCE_FILES} "external/Resources/${RESOURCE_DIR_FILES}")
      ELSEIF(NOT ${CHILD} MATCHES "\\.DS_Store|.*\\.am")
        SET_SOURCE_FILES_PROPERTIES(
          "${RESOURCE_DIR}/${CHILD}"
          PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${RESOURCE_DIR_NAME}
        )
        SET(RESOURCE_FILES ${RESOURCE_FILES} "external/Resources/${RESOURCE_DIR_NAME}/${CHILD}")
      ENDIF()
    endforeach()
  else(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") #WORK ON
    INSTALL( DIRECTORY ${RESOURCE_DIR} DESTINATION "${DATA_DIR}" PATTERN "*.am" EXCLUDE)
  endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
endforeach()

add_executable(${APP_NAME} MACOSX_BUNDLE
  src/main.cpp
  ${RESOURCE_FILES}
)

target_include_directories(${APP_NAME} PRIVATE 
  ${CMAKE_SOURCE_DIR}/external
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/external/nativefiledialog/src/include
  ${CMAKE_SOURCE_DIR}/external/libsamplerate/src/
)

target_link_directories(${APP_NAME} PUBLIC 
  /usr/local/lib
  ${CMAKE_SOURCE_DIR}/external/nativefiledialog/build/lib/release/x64
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
  ${CMAKE_SOURCE_DIR}/external/libsamplerate/build/libsamplerate.a 
  libnfd.a
) 

if(APPLE)
  target_link_libraries(ecLib PUBLIC ${Appkit})
endif()

if(LINUX)
  target_link_libraries(ecLib PUBLIC ${GTK3_LIBRARIES})
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

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  set_target_properties(${APP_NAME} PROPERTIES
  MACOSX_BUNDLE_BUNDLE_NAME "${APP_NAME}"
  MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../externalResources/osx/MacOSXBundleInfo.plist.in
  )


endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(${CMAKE_CURRENT_LIST_DIR}bin/resources/libsndfile.dylib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/buildTest)

add_custom_command(TARGET ${APP_NAME}
    POST_BUILD COMMAND 
    ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@executable_path/../Resources/" #libsndfile.1.dylib"
    $<TARGET_FILE:${APP_NAME}>)
