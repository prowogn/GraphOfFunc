#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "unofficial::tinyexpr::tinyexpr" for configuration "Debug"
set_property(TARGET unofficial::tinyexpr::tinyexpr APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::tinyexpr::tinyexpr PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/tinyexpr.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/tinyexpr.dll"
  )

list(APPEND _cmake_import_check_targets unofficial::tinyexpr::tinyexpr )
list(APPEND _cmake_import_check_files_for_unofficial::tinyexpr::tinyexpr "${_IMPORT_PREFIX}/lib/tinyexpr.lib" "${_IMPORT_PREFIX}/bin/tinyexpr.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
