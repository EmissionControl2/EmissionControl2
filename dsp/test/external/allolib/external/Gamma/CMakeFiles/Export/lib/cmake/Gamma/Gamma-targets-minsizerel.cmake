#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Gamma" for configuration "MinSizeRel"
set_property(TARGET Gamma APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(Gamma PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_MINSIZEREL "/usr/local/lib/libsndfile.dylib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/libGamma.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Gamma )
list(APPEND _IMPORT_CHECK_FILES_FOR_Gamma "${_IMPORT_PREFIX}/lib/libGamma.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
