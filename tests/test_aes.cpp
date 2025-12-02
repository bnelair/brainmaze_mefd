/**
 * @file test_aes.cpp
 * @brief AES-128 tests
 */

#include <brainmaze_mefd/aes.hpp>
#include <iostream>
#include <cstring>

using namespace brainmaze_mefd;

bool test_aes() {
    bool all_passed = true;
    
    // Test 1: Encrypt and decrypt round-trip
    {
        const char* password = "test_password";
        ui1 plaintext[16] = {'H','e','l','l','o',' ','M','E','F',' ','3','.','0','!','!','!'};
        ui1 ciphertext[16];
        ui1 decrypted[16];
        
        AES128::encrypt(plaintext, ciphertext, password);
        AES128::decrypt(ciphertext, decrypted, password);
        
        if (std::memcmp(plaintext, decrypted, 16) != 0) {
            std::cout << "  ERROR: AES encrypt/decrypt round-trip failed" << std::endl;
            all_passed = false;
        } else {
            std::cout << "  AES encrypt/decrypt round-trip: OK" << std::endl;
        }
    }
    
    // Test 2: Pre-expanded key
    {
        const char* password = "another_key";
        ui1 expanded_key[AES128::EXPANDED_KEY_SIZE];
        AES128::key_expansion(password, expanded_key);
        
        ui1 plaintext[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        ui1 ciphertext[16];
        ui1 decrypted[16];
        
        AES128::encrypt_with_key(plaintext, ciphertext, expanded_key);
        AES128::decrypt_with_key(ciphertext, decrypted, expanded_key);
        
        if (std::memcmp(plaintext, decrypted, 16) != 0) {
            std::cout << "  ERROR: AES with expanded key failed" << std::endl;
            all_passed = false;
        } else {
            std::cout << "  AES with expanded key: OK" << std::endl;
        }
    }
    
    // Test 3: Verify cipher text is different from plain text
    {
        const char* password = "key123";
        ui1 plaintext[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        ui1 ciphertext[16];
        
        AES128::encrypt(plaintext, ciphertext, password);
        
        if (std::memcmp(plaintext, ciphertext, 16) == 0) {
            std::cout << "  ERROR: Ciphertext should not equal plaintext" << std::endl;
            all_passed = false;
        } else {
            std::cout << "  AES produces different ciphertext: OK" << std::endl;
        }
    }
    
    // Test 4: In-place encryption
    {
        const char* password = "inplace_key";
        ui1 original[16] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE,
                           0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
        ui1 buffer[16];
        std::memcpy(buffer, original, 16);
        
        // Encrypt in place
        AES128::encrypt(buffer, buffer, password);
        
        // Decrypt in place
        AES128::decrypt(buffer, buffer, password);
        
        if (std::memcmp(original, buffer, 16) != 0) {
            std::cout << "  ERROR: In-place encryption/decryption failed" << std::endl;
            all_passed = false;
        } else {
            std::cout << "  AES in-place encryption: OK" << std::endl;
        }
    }
    
    return all_passed;
}
