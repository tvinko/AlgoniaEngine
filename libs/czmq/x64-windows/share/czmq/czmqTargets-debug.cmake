#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "czmq" for configuration "Debug"
set_property(TARGET czmq APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(czmq PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/czmq.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/libczmq.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS czmq )
list(APPEND _IMPORT_CHECK_FILES_FOR_czmq "${_IMPORT_PREFIX}/debug/lib/czmq.lib" "${_IMPORT_PREFIX}/debug/bin/libczmq.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
