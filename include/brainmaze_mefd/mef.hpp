/**
 * @file mef.hpp
 * @brief Main header for brainmaze_mefd library
 * 
 * Modern C++ implementation of MEF 3.0 (Multiscale Electrophysiology Format)
 * 
 * This library provides:
 * - Full MEF 3.0 format support
 * - Time series channel reading/writing
 * - Lossless (and lossy) compression via RED codec
 * - AES-128 encryption
 * - CRC-32 data integrity checking
 * - Full backward compatibility with meflib, pymef, and mef-tools files
 */

#ifndef BRAINMAZE_MEFD_MEF_HPP
#define BRAINMAZE_MEFD_MEF_HPP

// Core types and constants
#include "types.hpp"
#include "constants.hpp"
#include "structures.hpp"

// Utility modules
#include "crc.hpp"
#include "aes.hpp"
#include "sha256.hpp"
#include "red.hpp"

// High-level API
#include "mef_reader.hpp"
#include "mef_writer.hpp"

/**
 * @namespace brainmaze_mefd
 * @brief Modern MEF 3.0 library namespace
 * 
 * Contains all classes and functions for working with MEF 3.0 files.
 * 
 * @section example Basic Usage Example
 * @code{.cpp}
 * #include <brainmaze_mefd/mef.hpp>
 * #include <vector>
 * 
 * using namespace brainmaze_mefd;
 * 
 * int main() {
 *     // Create a MEF writer
 *     MefWriter writer("/path/to/session.mefd", true);
 *     writer.set_data_units("mV");
 *     writer.set_mef_block_len(1000);
 *     
 *     // Generate some test data
 *     std::vector<sf8> data(60000);
 *     for (int i = 0; i < data.size(); ++i) {
 *         data[i] = sin(2 * M_PI * i / 1000.0);
 *     }
 *     
 *     // Write data at 1 kHz
 *     int64_t start_time = 1700000000000000LL;  // microseconds since epoch
 *     writer.write_data(data, "channel_1", start_time, 1000.0);
 *     
 *     // Read back the data
 *     MefReader reader("/path/to/session.mefd");
 *     auto read_data = reader.get_data("channel_1");
 *     
 *     return 0;
 * }
 * @endcode
 */
namespace brainmaze_mefd {

/**
 * @brief Get library version string
 * @return Version string (e.g., "1.0.0")
 */
inline const char* get_version() {
    return "1.0.0";
}

/**
 * @brief Get MEF format version string
 * @return MEF version string (e.g., "3.0")
 */
inline const char* get_mef_version() {
    return "3.0";
}

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_MEF_HPP
