/**
 * @file types.hpp
 * @brief Fundamental type definitions for MEF 3.0
 * 
 * Specification for Multiscale Electrophysiology Format (MEF) version 3.0
 * Copyright 2021, Mayo Foundation, Rochester MN. All rights reserved.
 * 
 * This file defines portable type aliases for consistent data sizes across platforms.
 * These types match the original meflib definitions for full compatibility.
 */

#ifndef BRAINMAZE_MEFD_TYPES_HPP
#define BRAINMAZE_MEFD_TYPES_HPP

#include <cstdint>
#include <cstddef>

namespace brainmaze_mefd {

// Signed integer types
using si1 = char;           // 8-bit signed integer (char for string compatibility)
using si2 = int16_t;        // 16-bit signed integer
using si4 = int32_t;        // 32-bit signed integer
using si8 = int64_t;        // 64-bit signed integer

// Unsigned integer types
using ui1 = uint8_t;        // 8-bit unsigned integer
using ui2 = uint16_t;       // 16-bit unsigned integer
using ui4 = uint32_t;       // 32-bit unsigned integer
using ui8 = uint64_t;       // 64-bit unsigned integer

// Floating point types
using sf4 = float;          // 32-bit floating point
using sf8 = double;         // 64-bit floating point
using sf16 = long double;   // Extended precision floating point

// Boolean type (MEF uses specific values)
constexpr si4 MEF_TRUE = 1;
constexpr si4 MEF_UNKNOWN = 0;
constexpr si4 MEF_FALSE = -1;

// Endianness constants
constexpr ui1 MEF_BIG_ENDIAN = 0;
constexpr ui1 MEF_LITTLE_ENDIAN = 1;

/**
 * @brief Determine CPU endianness at runtime
 * @return MEF_LITTLE_ENDIAN or MEF_BIG_ENDIAN
 */
inline ui1 cpu_endianness() noexcept {
    union {
        ui4 i;
        ui1 c[4];
    } test = {0x01020304};
    return (test.c[0] == 0x04) ? MEF_LITTLE_ENDIAN : MEF_BIG_ENDIAN;
}

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_TYPES_HPP
