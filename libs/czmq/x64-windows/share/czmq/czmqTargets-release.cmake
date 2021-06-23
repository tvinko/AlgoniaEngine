#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "czmq" for configuration "Release"
set_property(TARGET czmq APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(czmq PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/czmq.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libczmq.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS czmq )
list(APPEND _IMPORT_CHECK_FILES_FOR_czmq "${_IMPORT_PREFIX}/lib/czmq.lib" "${_IMPORT_PREFIX}/bin/libczmq.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
