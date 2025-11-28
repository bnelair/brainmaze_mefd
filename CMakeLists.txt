cmake_minimum_required(VERSION 3.14)
project(brainmaze_mefd
    VERSION 0.1.0
    DESCRIPTION "Modern C++ implementation of brainmaze_mefd"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Library
add_library(brainmaze_mefd_lib
    src/brainmaze_mefd.cpp
)
target_include_directories(brainmaze_mefd_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Main executable
add_executable(brainmaze_mefd
    src/main.cpp
)
target_link_libraries(brainmaze_mefd PRIVATE brainmaze_mefd_lib)

# Testing
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    
    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Test executable
    add_executable(brainmaze_mefd_tests
        tests/test_main.cpp
        tests/test_brainmaze_mefd.cpp
    )
    target_link_libraries(brainmaze_mefd_tests PRIVATE
        brainmaze_mefd_lib
        GTest::gtest
    )
    
    include(GoogleTest)
    gtest_discover_tests(brainmaze_mefd_tests)
endif()
