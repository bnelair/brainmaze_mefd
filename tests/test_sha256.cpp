/**
 * @file test_sha256.cpp
 * @brief SHA-256 tests
 */

#include <brainmaze_mefd/sha256.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace brainmaze_mefd;

// Helper function to convert digest to hex string
std::string digest_to_hex(const SHA256::Digest& digest) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : digest) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

bool test_sha256() {
    bool all_passed = true;
    
    // Test 1: Empty string - known hash value
    {
        auto digest = SHA256::hash("");
        std::string hex = digest_to_hex(digest);
        
        // SHA-256 of empty string
        const char* expected = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
        
        std::cout << "  SHA256(''): " << hex << std::endl;
        
        if (hex != expected) {
            std::cout << "  ERROR: Expected: " << expected << std::endl;
            all_passed = false;
        }
    }
    
    // Test 2: "abc" - known hash value
    {
        auto digest = SHA256::hash("abc");
        std::string hex = digest_to_hex(digest);
        
        // SHA-256 of "abc"
        const char* expected = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
        
        std::cout << "  SHA256('abc'): " << hex << std::endl;
        
        if (hex != expected) {
            std::cout << "  ERROR: Expected: " << expected << std::endl;
            all_passed = false;
        }
    }
    
    // Test 3: Longer string
    {
        auto digest = SHA256::hash("The quick brown fox jumps over the lazy dog");
        std::string hex = digest_to_hex(digest);
        
        const char* expected = "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592";
        
        std::cout << "  SHA256('The quick...'): " << hex << std::endl;
        
        if (hex != expected) {
            std::cout << "  ERROR: Expected: " << expected << std::endl;
            all_passed = false;
        }
    }
    
    // Test 4: Streaming API
    {
        SHA256::Context ctx;
        ctx.update(reinterpret_cast<const ui1*>("The quick "), 10);
        ctx.update(reinterpret_cast<const ui1*>("brown fox "), 10);
        ctx.update(reinterpret_cast<const ui1*>("jumps over "), 11);
        ctx.update(reinterpret_cast<const ui1*>("the lazy dog"), 12);
        auto digest = ctx.finalize();
        
        std::string hex = digest_to_hex(digest);
        const char* expected = "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592";
        
        if (hex != expected) {
            std::cout << "  ERROR: Streaming SHA256 mismatch" << std::endl;
            all_passed = false;
        } else {
            std::cout << "  SHA256 streaming API: OK" << std::endl;
        }
    }
    
    return all_passed;
}
