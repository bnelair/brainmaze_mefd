#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "brainmaze_mefd::brainmaze_mefd" for configuration "Release"
set_property(TARGET brainmaze_mefd::brainmaze_mefd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(brainmaze_mefd::brainmaze_mefd PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libbrainmaze_mefd.so.1.0.0"
  IMPORTED_SONAME_RELEASE "libbrainmaze_mefd.so.1"
  )

list(APPEND _cmake_import_check_targets brainmaze_mefd::brainmaze_mefd )
list(APPEND _cmake_import_check_files_for_brainmaze_mefd::brainmaze_mefd "${_IMPORT_PREFIX}/lib/libbrainmaze_mefd.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
