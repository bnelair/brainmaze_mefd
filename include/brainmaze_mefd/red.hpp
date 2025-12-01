/**
 * @file red.hpp
 * @brief RED (Range Encoded Differences) codec for MEF 3.0
 * 
 * Implements the RED compression algorithm for time series data.
 * RED uses range encoding of differences between consecutive samples
 * for efficient lossless compression.
 */

#ifndef BRAINMAZE_MEFD_RED_HPP
#define BRAINMAZE_MEFD_RED_HPP

#include "types.hpp"
#include "constants.hpp"
#include "structures.hpp"
#include <vector>
#include <array>

namespace brainmaze_mefd {

/**
 * @brief RED compression/decompression codec
 * 
 * Provides lossless compression for time series data using the
 * Range Encoded Differences algorithm.
 */
class REDCodec {
public:
    /**
     * @brief Compression parameters
     */
    struct CompressionParams {
        ui1 mode = RED_LOSSLESS_COMPRESSION;
        si1 encryption_level = RED_ENCRYPTION_LEVEL_DEFAULT;
        bool discontinuity = true;
        bool detrend_data = false;
        sf8 goal_compression_ratio = RED_GOAL_COMPRESSION_RATIO_DEFAULT;
        sf8 goal_mean_residual_ratio = RED_GOAL_MEAN_RESIDUAL_RATIO_DEFAULT;
        sf8 goal_tolerance = RED_GOAL_TOLERANCE_DEFAULT;
        si4 max_rounds = RED_MAXIMUM_ROUNDS_PER_BLOCK_DEFAULT;
        bool require_normality = true;
        sf8 normal_correlation = RED_NORMAL_CORRELATION_DEFAULT;
    };
    
    /**
     * @brief Compression result
     */
    struct CompressionResult {
        std::vector<ui1> compressed_data;
        REDBlockHeader block_header;
        TimeSeriesIndex index;
        bool success = false;
    };
    
    /**
     * @brief Decompression result
     */
    struct DecompressionResult {
        std::vector<si4> samples;
        REDBlockHeader block_header;
        bool success = false;
    };
    
    /**
     * @brief Compress a block of samples
     * @param samples Input samples (si4)
     * @param num_samples Number of samples
     * @param start_time Block start time (uUTC)
     * @param params Compression parameters
     * @return CompressionResult containing compressed data and metadata
     */
    static CompressionResult compress(const si4* samples, ui4 num_samples, 
                                      si8 start_time, const CompressionParams& params);
    
    /**
     * @brief Compress a block of samples with default parameters
     * @param samples Input samples (si4)
     * @param num_samples Number of samples
     * @param start_time Block start time (uUTC)
     * @return CompressionResult containing compressed data and metadata
     */
    static CompressionResult compress(const si4* samples, ui4 num_samples, si8 start_time) {
        return compress(samples, num_samples, start_time, CompressionParams{});
    }
    
    /**
     * @brief Decompress a block
     * @param compressed_data Pointer to compressed data (including header)
     * @param compressed_size Size of compressed data
     * @param password_data Optional password data for decryption
     * @return DecompressionResult containing decompressed samples
     */
    static DecompressionResult decompress(const ui1* compressed_data, 
                                          size_t compressed_size,
                                          const PasswordData* password_data = nullptr);
    
    /**
     * @brief Decompress using pre-parsed block header
     * @param block_header Pre-parsed block header
     * @param compressed_data Pointer to compressed difference data (after header)
     * @param password_data Optional password data for decryption
     * @return DecompressionResult containing decompressed samples
     */
    static DecompressionResult decompress(const REDBlockHeader& block_header,
                                          const ui1* compressed_data,
                                          const PasswordData* password_data = nullptr);

    /**
     * @brief Calculate required buffer size for compression
     * @param num_samples Number of samples to compress
     * @return Maximum compressed size in bytes
     */
    static size_t max_compressed_size(ui4 num_samples) {
        return static_cast<size_t>(RED_MAX_COMPRESSED_BYTES(num_samples, 1));
    }
    
    /**
     * @brief Find min/max sample values
     * @param samples Sample array
     * @param num_samples Number of samples
     * @param min_val Output minimum value
     * @param max_val Output maximum value
     */
    static void find_extrema(const si4* samples, ui4 num_samples, 
                             si4& min_val, si4& max_val);

private:
    // Range encoding constants
    static constexpr ui4 RANGE_TOP_VALUE = TOP_VALUE;
    static constexpr ui4 RANGE_BOTTOM_VALUE = BOTTOM_VALUE;
    
    // Internal encoding/decoding
    static void encode_differences(const si4* samples, ui4 num_samples,
                                   std::vector<ui1>& output,
                                   std::array<ui4, 257>& counts);
    
    static void decode_differences(const ui1* input, ui4 diff_bytes,
                                   const std::array<ui1, RED_BLOCK_STATISTICS_BYTES>& statistics,
                                   si4* output, ui4 num_samples);
    
    // Statistics computation
    static void compute_statistics(const si4* differences, ui4 num_samples,
                                   std::array<ui1, RED_BLOCK_STATISTICS_BYTES>& stats);
    
    // CDF table for normality testing
    static const std::array<sf8, RED_NORMAL_CDF_TABLE_ENTRIES> s_normal_cdf_table;
};

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_RED_HPP
