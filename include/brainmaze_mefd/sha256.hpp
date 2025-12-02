/**
 * @file sha256.hpp
 * @brief SHA-256 implementation for MEF 3.0
 * 
 * Implementation of SHA-256 hash algorithm for MEF password validation.
 * Based on FIPS 180-4.
 */

#ifndef BRAINMAZE_MEFD_SHA256_HPP
#define BRAINMAZE_MEFD_SHA256_HPP

#include "types.hpp"
#include <array>
#include <string>

namespace brainmaze_mefd {

/**
 * @brief SHA-256 hash calculator
 * 
 * Provides SHA-256 computation for password validation and data integrity.
 */
class SHA256 {
public:
    static constexpr size_t DIGEST_SIZE = 32;  // 256 bits
    static constexpr size_t BLOCK_SIZE = 64;   // 512 bits

    using Digest = std::array<ui1, DIGEST_SIZE>;

    /**
     * @brief Calculate SHA-256 hash of data
     * @param data Pointer to input data
     * @param length Length of data in bytes
     * @return 32-byte SHA-256 digest
     */
    static Digest hash(const ui1* data, ui4 length);

    /**
     * @brief Calculate SHA-256 hash of a string
     * @param str Input string
     * @return 32-byte SHA-256 digest
     */
    static Digest hash(const std::string& str);

    /**
     * @brief SHA-256 context for streaming updates
     */
    class Context {
    public:
        Context();
        
        /**
         * @brief Update hash with more data
         * @param data Pointer to data
         * @param length Length in bytes
         */
        void update(const ui1* data, ui4 length);

        /**
         * @brief Finalize and return the hash
         * @return 32-byte SHA-256 digest
         */
        Digest finalize();

    private:
        void transform(const ui1* message, ui4 block_nb);
        
        ui4 m_total_len;
        ui4 m_len;
        std::array<ui1, 2 * BLOCK_SIZE> m_block;
        std::array<ui4, 8> m_h;
    };

private:
    // Initial hash values (first 32 bits of fractional parts of square roots of first 8 primes)
    static const std::array<ui4, 8> s_h0;
    
    // Round constants (first 32 bits of fractional parts of cube roots of first 64 primes)
    static const std::array<ui4, 64> s_k;
};

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_SHA256_HPP
