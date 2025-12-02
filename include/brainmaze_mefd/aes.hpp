/**
 * @file aes.hpp
 * @brief AES-128 encryption/decryption for MEF 3.0
 * 
 * Implementation of AES-128 encryption for MEF data protection.
 * Based on the AES standard (FIPS-197).
 */

#ifndef BRAINMAZE_MEFD_AES_HPP
#define BRAINMAZE_MEFD_AES_HPP

#include "types.hpp"
#include <array>
#include <string>
#include <cstring>

namespace brainmaze_mefd {

/**
 * @brief AES-128 encryption/decryption class
 * 
 * Provides AES-128 encryption and decryption for MEF 3.0 files.
 * This implementation is specifically for 128-bit keys only.
 */
class AES128 {
public:
    // AES-128 constants
    static constexpr int NR = 10;    // Number of rounds
    static constexpr int NK = 4;     // Number of 32-bit words in key
    static constexpr int NB = 4;     // Number of columns in state
    static constexpr int BLOCK_SIZE = 16;
    static constexpr int KEY_SIZE = 16;
    static constexpr int EXPANDED_KEY_SIZE = 176;  // (NR + 1) * NK * NB

    /**
     * @brief Encrypt a 16-byte block
     * @param input 16-byte input plaintext
     * @param output 16-byte output ciphertext
     * @param password Password string (max 16 characters)
     */
    static void encrypt(const ui1* input, ui1* output, const char* password);

    /**
     * @brief Encrypt a 16-byte block with pre-expanded key
     * @param input 16-byte input plaintext
     * @param output 16-byte output ciphertext
     * @param expanded_key Pre-expanded 176-byte key
     */
    static void encrypt_with_key(const ui1* input, ui1* output, const ui1* expanded_key);

    /**
     * @brief Decrypt a 16-byte block
     * @param input 16-byte input ciphertext
     * @param output 16-byte output plaintext
     * @param password Password string (max 16 characters)
     */
    static void decrypt(const ui1* input, ui1* output, const char* password);

    /**
     * @brief Decrypt a 16-byte block with pre-expanded key
     * @param input 16-byte input ciphertext
     * @param output 16-byte output plaintext
     * @param expanded_key Pre-expanded 176-byte key
     */
    static void decrypt_with_key(const ui1* input, ui1* output, const ui1* expanded_key);

    /**
     * @brief Expand a password into a round key array
     * @param password Password (16 bytes, zero-padded if shorter)
     * @param expanded_key Output 176-byte expanded key array
     */
    static void key_expansion(const char* password, ui1* expanded_key);

    /**
     * @brief Get the S-box table
     * @return Reference to the S-box table
     */
    static const std::array<ui1, 256>& get_sbox();

    /**
     * @brief Get the inverse S-box table
     * @return Reference to the inverse S-box table
     */
    static const std::array<ui1, 256>& get_rsbox();

private:
    // Core AES operations
    static void cipher(const ui1* input, ui1* output, const ui1* round_key);
    static void inv_cipher(const ui1* input, ui1* output, const ui1* round_key);
    
    // Round operations
    static void add_round_key(int round, ui1 state[4][4], const ui1* round_key);
    static void sub_bytes(ui1 state[4][4]);
    static void inv_sub_bytes(ui1 state[4][4]);
    static void shift_rows(ui1 state[4][4]);
    static void inv_shift_rows(ui1 state[4][4]);
    static void mix_columns(ui1 state[4][4]);
    static void inv_mix_columns(ui1 state[4][4]);
    
    // Helper macros as inline functions
    static inline ui1 xtime(ui1 x) { return ((x << 1) ^ (((x >> 7) & 1) * 0x1b)); }
    static inline ui1 multiply(ui1 x, ui1 y);
    
    // Static lookup tables
    static const std::array<ui1, 256> s_sbox;
    static const std::array<ui1, 256> s_rsbox;
    static const std::array<ui1, 256> s_rcon;
};

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_AES_HPP
