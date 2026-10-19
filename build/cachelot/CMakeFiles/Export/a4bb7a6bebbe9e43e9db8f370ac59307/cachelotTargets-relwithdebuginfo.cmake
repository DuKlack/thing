#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebugInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cachelot" for configuration "RelWithDebugInfo"
set_property(TARGET cachelot APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBUGINFO)
set_target_properties(cachelot PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBUGINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBUGINFO "${_IMPORT_PREFIX}/lib/libcachelot.a"
  )

list(APPEND _cmake_import_check_targets cachelot )
list(APPEND _cmake_import_check_files_for_cachelot "${_IMPORT_PREFIX}/lib/libcachelot.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
