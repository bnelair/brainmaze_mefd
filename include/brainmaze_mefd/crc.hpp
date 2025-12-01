/**
 * @file crc.hpp
 * @brief CRC-32 implementation for MEF 3.0
 * 
 * Implements Koopman32 CRC for data integrity checking.
 */

#ifndef BRAINMAZE_MEFD_CRC_HPP
#define BRAINMAZE_MEFD_CRC_HPP

#include "types.hpp"
#include <array>

namespace brainmaze_mefd {

/**
 * @brief CRC-32 calculator using Koopman32 polynomial
 * 
 * This class provides CRC-32 computation for MEF 3.0 data integrity verification.
 * Uses the Koopman32 polynomial (0xEB31D82E) for optimal error detection.
 */
class CRC32 {
public:
    static constexpr ui4 CRC_START_VALUE = 0xFFFFFFFF;
    static constexpr size_t TABLE_ENTRIES = 256;
    static constexpr ui4 KOOPMAN32 = 0xEB31D82E;

    /**
     * @brief Calculate CRC-32 for a block of data
     * @param data Pointer to data
     * @param length Number of bytes
     * @return CRC-32 value
     */
    static ui4 calculate(const ui1* data, size_t length);

    /**
     * @brief Update running CRC with additional data
     * @param data Pointer to new data
     * @param length Number of bytes
     * @param current_crc Current CRC value
     * @return Updated CRC-32 value
     */
    static ui4 update(const ui1* data, size_t length, ui4 current_crc);

    /**
     * @brief Validate CRC against expected value
     * @param data Pointer to data
     * @param length Number of bytes
     * @param expected_crc Expected CRC value
     * @return true if CRC matches, false otherwise
     */
    static bool validate(const ui1* data, size_t length, ui4 expected_crc);

    /**
     * @brief Get the CRC lookup table
     * @return Reference to the static CRC table
     */
    static const std::array<ui4, TABLE_ENTRIES>& get_table();

private:
    static std::array<ui4, TABLE_ENTRIES> initialize_table();
    static std::array<ui4, TABLE_ENTRIES> s_table;
    static bool s_initialized;
};

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_CRC_HPP
