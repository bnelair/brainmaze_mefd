/**
 * @file test_main.cpp
 * @brief Test runner for brainmaze_mefd
 */

#include <iostream>
#include <cstdlib>

// Forward declarations of test functions
bool test_crc();
bool test_aes();
bool test_sha256();
bool test_red();
bool test_mef();

int main() {
    std::cout << "=== brainmaze_mefd Test Suite ===" << std::endl;
    
    int failures = 0;
    
    std::cout << "\n--- CRC Tests ---" << std::endl;
    if (!test_crc()) {
        failures++;
        std::cout << "FAILED: CRC tests" << std::endl;
    } else {
        std::cout << "PASSED: CRC tests" << std::endl;
    }
    
    std::cout << "\n--- AES Tests ---" << std::endl;
    if (!test_aes()) {
        failures++;
        std::cout << "FAILED: AES tests" << std::endl;
    } else {
        std::cout << "PASSED: AES tests" << std::endl;
    }
    
    std::cout << "\n--- SHA256 Tests ---" << std::endl;
    if (!test_sha256()) {
        failures++;
        std::cout << "FAILED: SHA256 tests" << std::endl;
    } else {
        std::cout << "PASSED: SHA256 tests" << std::endl;
    }
    
    std::cout << "\n--- RED Tests ---" << std::endl;
    if (!test_red()) {
        failures++;
        std::cout << "FAILED: RED tests" << std::endl;
    } else {
        std::cout << "PASSED: RED tests" << std::endl;
    }
    
    std::cout << "\n--- MEF Tests ---" << std::endl;
    if (!test_mef()) {
        failures++;
        std::cout << "FAILED: MEF tests" << std::endl;
    } else {
        std::cout << "PASSED: MEF tests" << std::endl;
    }
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    if (failures == 0) {
        std::cout << "All tests passed!" << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cout << failures << " test(s) failed!" << std::endl;
        return EXIT_FAILURE;
    }
}
