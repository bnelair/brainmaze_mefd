cmake_minimum_required(VERSION 3.15)
project(brainmaze_mefd VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build test executables" ON)
option(BUILD_PYTHON "Build Python bindings" ON)

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source files
set(SOURCES
    src/crc.cpp
    src/aes.cpp
    src/sha256.cpp
    src/red.cpp
    src/mef_reader.cpp
    src/mef_writer.cpp
)

# Header files
set(HEADERS
    include/brainmaze_mefd/types.hpp
    include/brainmaze_mefd/constants.hpp
    include/brainmaze_mefd/structures.hpp
    include/brainmaze_mefd/crc.hpp
    include/brainmaze_mefd/aes.hpp
    include/brainmaze_mefd/sha256.hpp
    include/brainmaze_mefd/red.hpp
    include/brainmaze_mefd/mef_reader.hpp
    include/brainmaze_mefd/mef_writer.hpp
    include/brainmaze_mefd/mef.hpp
)

# Library target
add_library(brainmaze_mefd ${SOURCES})
target_include_directories(brainmaze_mefd
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(brainmaze_mefd PRIVATE _WIN32)
endif()

# Set library properties
set_target_properties(brainmaze_mefd PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${HEADERS}"
)

# Installation
include(GNUInstallDirs)
install(TARGETS brainmaze_mefd
    EXPORT brainmaze_mefdTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brainmaze_mefd
)

# Export targets
install(EXPORT brainmaze_mefdTargets
    FILE brainmaze_mefdTargets.cmake
    NAMESPACE brainmaze_mefd::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/brainmaze_mefd
)

# Package configuration
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/brainmaze_mefdConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/brainmaze_mefdConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/brainmaze_mefd
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/brainmaze_mefdConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/brainmaze_mefdConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/brainmaze_mefdConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/brainmaze_mefd
)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Python bindings (optional)
if(BUILD_PYTHON)
    find_package(Python3 COMPONENTS Interpreter Development)
    if(Python3_FOUND)
        # Try to find pybind11
        execute_process(
            COMMAND ${Python3_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
            OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(PYBIND11_CMAKE_DIR)
            list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_CMAKE_DIR}")
            find_package(pybind11 CONFIG)
            if(pybind11_FOUND)
                add_subdirectory(python)
            else()
                message(STATUS "pybind11 not found, Python bindings will not be built")
            endif()
        else()
            message(STATUS "pybind11 not installed, Python bindings will not be built")
        endif()
    else()
        message(STATUS "Python3 not found, Python bindings will not be built")
    endif()
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "brainmaze_mefd ${PROJECT_VERSION} Configuration Summary")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build shared libs: ${BUILD_SHARED_LIBS}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build Python bindings: ${BUILD_PYTHON}")
message(STATUS "")
