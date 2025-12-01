# Install script for directory: /home/runner/work/brainmaze_mefd/brainmaze_mefd

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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libbrainmaze_mefd.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libbrainmaze_mefd.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/libbrainmaze_mefd.so.1.0.0"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/libbrainmaze_mefd.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libbrainmaze_mefd.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libbrainmaze_mefd.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/libbrainmaze_mefd.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/brainmaze_mefd" TYPE FILE FILES
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/types.hpp"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/constants.hpp"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/structures.hpp"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/crc.hpp"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/aes.hpp"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/sha256.hpp"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/red.hpp"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/mef_reader.hpp"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/mef_writer.hpp"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/include/brainmaze_mefd/mef.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/brainmaze_mefd/brainmaze_mefdTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/brainmaze_mefd/brainmaze_mefdTargets.cmake"
         "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/CMakeFiles/Export/fa1f1b01dafa1e9552d2adae4b33e98a/brainmaze_mefdTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/brainmaze_mefd/brainmaze_mefdTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/brainmaze_mefd/brainmaze_mefdTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/brainmaze_mefd" TYPE FILE FILES "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/CMakeFiles/Export/fa1f1b01dafa1e9552d2adae4b33e98a/brainmaze_mefdTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/brainmaze_mefd" TYPE FILE FILES "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/CMakeFiles/Export/fa1f1b01dafa1e9552d2adae4b33e98a/brainmaze_mefdTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/brainmaze_mefd" TYPE FILE FILES
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/brainmaze_mefdConfig.cmake"
    "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/brainmaze_mefdConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/tests/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/brainmaze_mefd/brainmaze_mefd/_codeql_build_dir/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
