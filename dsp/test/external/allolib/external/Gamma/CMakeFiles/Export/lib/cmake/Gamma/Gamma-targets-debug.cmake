#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Gamma" for configuration "Debug"
set_property(TARGET Gamma APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Gamma PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "/usr/local/lib/libsndfile.dylib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libGammad.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Gamma )
list(APPEND _IMPORT_CHECK_FILES_FOR_Gamma "${_IMPORT_PREFIX}/lib/libGammad.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
