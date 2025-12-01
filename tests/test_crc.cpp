/**
 * @file test_crc.cpp
 * @brief CRC-32 tests
 */

#include <brainmaze_mefd/crc.hpp>
#include <iostream>
#include <cstring>

using namespace brainmaze_mefd;

bool test_crc() {
    bool all_passed = true;
    
    // Test 1: Known CRC value for "123456789"
    {
        const char* test_str = "123456789";
        ui4 crc = CRC32::calculate(reinterpret_cast<const ui1*>(test_str), 9);
        
        // Note: The expected value depends on the specific CRC polynomial
        // Koopman32 polynomial should give consistent results
        std::cout << "  CRC of '123456789': 0x" << std::hex << crc << std::dec << std::endl;
        
        // Verify CRC can be updated incrementally
        ui4 crc1 = CRC32::calculate(reinterpret_cast<const ui1*>(test_str), 4);
        ui4 crc2 = CRC32::update(reinterpret_cast<const ui1*>(test_str + 4), 5, crc1);
        
        if (crc != crc2) {
            std::cout << "  ERROR: Incremental CRC doesn't match: 0x" 
                      << std::hex << crc2 << " != 0x" << crc << std::dec << std::endl;
            all_passed = false;
        }
    }
    
    // Test 2: Empty data
    {
        ui4 crc = CRC32::calculate(nullptr, 0);
        ui4 expected = CRC32::CRC_START_VALUE;
        
        if (crc != expected) {
            std::cout << "  ERROR: Empty CRC mismatch" << std::endl;
            all_passed = false;
        }
    }
    
    // Test 3: Validation function
    {
        const char* test_str = "Hello, MEF!";
        ui4 crc = CRC32::calculate(reinterpret_cast<const ui1*>(test_str), std::strlen(test_str));
        
        bool valid = CRC32::validate(reinterpret_cast<const ui1*>(test_str), 
                                     std::strlen(test_str), crc);
        if (!valid) {
            std::cout << "  ERROR: CRC validation failed" << std::endl;
            all_passed = false;
        }
        
        // Test with wrong CRC
        bool invalid = CRC32::validate(reinterpret_cast<const ui1*>(test_str), 
                                       std::strlen(test_str), crc + 1);
        if (invalid) {
            std::cout << "  ERROR: CRC should not validate with wrong value" << std::endl;
            all_passed = false;
        }
    }
    
    return all_passed;
}
