cmake_minimum_required(VERSION 3.15)

# Test executable
add_executable(test_brainmaze_mefd
    test_crc.cpp
    test_aes.cpp
    test_sha256.cpp
    test_red.cpp
    test_mef.cpp
    test_main.cpp
)

target_link_libraries(test_brainmaze_mefd PRIVATE brainmaze_mefd)

# Add tests
add_test(NAME test_brainmaze_mefd COMMAND test_brainmaze_mefd)
