/**
 * @file constants.hpp
 * @brief MEF 3.0 format constants and definitions
 * 
 * Specification for Multiscale Electrophysiology Format (MEF) version 3.0
 * Copyright 2021, Mayo Foundation, Rochester MN. All rights reserved.
 * 
 * This file contains all MEF 3.0 format constants including:
 * - Version information
 * - File type codes
 * - Size constants for all structures
 * - Encryption and password constants
 * - Recording time offset modes
 * - CRC modes
 */

#ifndef BRAINMAZE_MEFD_CONSTANTS_HPP
#define BRAINMAZE_MEFD_CONSTANTS_HPP

#include "types.hpp"

namespace brainmaze_mefd {

// ============================================================================
// Version Information
// ============================================================================
constexpr ui1 MEF_VERSION_MAJOR = 3;
constexpr ui1 MEF_VERSION_MINOR = 0;

// ============================================================================
// Miscellaneous Constants
// ============================================================================
constexpr si4 TYPE_BYTES = 5;
constexpr si4 UUID_BYTES = 16;
constexpr si4 NO_UUID = 0;
constexpr si4 TIME_STRING_BYTES = 32;
constexpr si4 MEF_BASE_FILE_NAME_BYTES = 256;    // utf8[63]
constexpr si4 MEF_SEGMENT_BASE_FILE_NAME_BYTES = MEF_BASE_FILE_NAME_BYTES + 8;
constexpr si4 MEF_FULL_FILE_NAME_BYTES = 4096;   // Reasonable max path length
constexpr ui1 PAD_BYTE_VALUE = 0x7e;             // ASCII tilde '~'
constexpr si4 FILE_NUMBERING_DIGITS = 6;
constexpr ui4 NO_TYPE_CODE = 0;
constexpr si4 MAXIMUM_GMT_OFFSET = 86400;
constexpr si4 MINIMUM_GMT_OFFSET = -86400;
constexpr si8 UNKNOWN_NUMBER_OF_ENTRIES = -1;
constexpr si8 UUTC_NO_ENTRY = static_cast<si8>(0x8000000000000000LL);
constexpr ui4 CRC_NO_ENTRY = 0;

// ============================================================================
// CRC Modes
// ============================================================================
constexpr ui4 CRC_IGNORE = 0;
constexpr ui4 CRC_VALIDATE = 1;
constexpr ui4 CRC_VALIDATE_ON_INPUT = 2;
constexpr ui4 CRC_VALIDATE_ON_OUTPUT = 4;
constexpr ui4 CRC_CALCULATE = 8;
constexpr ui4 CRC_CALCULATE_ON_INPUT = 16;
constexpr ui4 CRC_CALCULATE_ON_OUTPUT = 32;

// ============================================================================
// Encryption & Password Constants
// ============================================================================
constexpr si1 ENCRYPTION_LEVEL_NO_ENTRY = -128;
constexpr si1 NO_ENCRYPTION = 0;
constexpr si1 LEVEL_0_ENCRYPTION = NO_ENCRYPTION;
constexpr si1 LEVEL_1_ENCRYPTION = 1;
constexpr si1 LEVEL_2_ENCRYPTION = 2;
constexpr si1 LEVEL_0_ACCESS = LEVEL_0_ENCRYPTION;
constexpr si1 LEVEL_1_ACCESS = LEVEL_1_ENCRYPTION;
constexpr si1 LEVEL_2_ACCESS = LEVEL_2_ENCRYPTION;
constexpr si1 LEVEL_1_ENCRYPTION_DECRYPTED = -LEVEL_1_ENCRYPTION;
constexpr si1 LEVEL_2_ENCRYPTION_DECRYPTED = -LEVEL_2_ENCRYPTION;
constexpr si4 ENCRYPTION_BLOCK_BYTES = 16;       // AES-128
constexpr si4 ENCRYPTION_KEY_BYTES = 176;        // (AES_NR + 1) * AES_NK * AES_NB
constexpr si4 PASSWORD_BYTES = ENCRYPTION_BLOCK_BYTES;
constexpr si4 UTF8_PASSWORD_BYTES = PASSWORD_BYTES * 4;
constexpr si4 MAX_PASSWORD_CHARACTERS = PASSWORD_BYTES - 1;
constexpr si4 PASSWORD_VALIDATION_FIELD_BYTES = PASSWORD_BYTES;

// ============================================================================
// Recording Time Offset Modes & Constants
// ============================================================================
constexpr si8 RTO_USE_SYSTEM_TIME = -1;
constexpr ui4 RTO_IGNORE = 0;
constexpr ui4 RTO_APPLY = 1;
constexpr ui4 RTO_REMOVE = 2;
constexpr ui4 RTO_APPLY_ON_OUTPUT = 4;
constexpr ui4 RTO_APPLY_ON_INPUT = 8;
constexpr ui4 RTO_REMOVE_ON_OUTPUT = 16;
constexpr ui4 RTO_REMOVE_ON_INPUT = 32;
constexpr ui4 RTO_INPUT_ACTION = 1;
constexpr ui4 RTO_OUTPUT_ACTION = 2;

// ============================================================================
// Global Defaults
// ============================================================================
constexpr si4 MEF_GLOBALS_VERBOSE_DEFAULT = MEF_FALSE;
constexpr si8 MEF_GLOBALS_RECORDING_TIME_OFFSET_DEFAULT = 0;
constexpr ui4 MEF_GLOBALS_RECORDING_TIME_OFFSET_MODE_DEFAULT = 
    (RTO_APPLY_ON_OUTPUT | RTO_REMOVE_ON_INPUT);
constexpr si4 MEF_GLOBALS_GMT_OFFSET_DEFAULT = 0;
constexpr si8 MEF_GLOBALS_DST_START_TIME_DEFAULT = UUTC_NO_ENTRY;
constexpr si8 MEF_GLOBALS_DST_END_TIME_DEFAULT = UUTC_NO_ENTRY;
constexpr ui4 MEF_GLOBALS_BEHAVIOR_ON_FAIL_DEFAULT = 2;  // EXIT_ON_FAIL
constexpr ui4 MEF_GLOBALS_CRC_MODE_DEFAULT = CRC_CALCULATE_ON_OUTPUT;

// ============================================================================
// File Type Constants
// ============================================================================
constexpr char SESSION_DIRECTORY_TYPE_STRING[] = "mefd";
constexpr ui4 SESSION_DIRECTORY_TYPE_CODE = 0x6466656d;  // little endian

constexpr char SEGMENT_DIRECTORY_TYPE_STRING[] = "segd";
constexpr ui4 SEGMENT_DIRECTORY_TYPE_CODE = 0x64676573;

constexpr char RECORD_DATA_FILE_TYPE_STRING[] = "rdat";
constexpr ui4 RECORD_DATA_FILE_TYPE_CODE = 0x74616472;

constexpr char RECORD_INDICES_FILE_TYPE_STRING[] = "ridx";
constexpr ui4 RECORD_INDICES_FILE_TYPE_CODE = 0x78646972;

constexpr char TIME_SERIES_CHANNEL_DIRECTORY_TYPE_STRING[] = "timd";
constexpr ui4 TIME_SERIES_CHANNEL_DIRECTORY_TYPE_CODE = 0x646d6974;

constexpr char TIME_SERIES_METADATA_FILE_TYPE_STRING[] = "tmet";
constexpr ui4 TIME_SERIES_METADATA_FILE_TYPE_CODE = 0x74656d74;

constexpr char TIME_SERIES_DATA_FILE_TYPE_STRING[] = "tdat";
constexpr ui4 TIME_SERIES_DATA_FILE_TYPE_CODE = 0x74616474;

constexpr char TIME_SERIES_INDICES_FILE_TYPE_STRING[] = "tidx";
constexpr ui4 TIME_SERIES_INDICES_FILE_TYPE_CODE = 0x78646974;

// Video channel types (included for compatibility, but not actively supported)
constexpr char VIDEO_CHANNEL_DIRECTORY_TYPE_STRING[] = "vidd";
constexpr ui4 VIDEO_CHANNEL_DIRECTORY_TYPE_CODE = 0x64646976;

constexpr char VIDEO_METADATA_FILE_TYPE_STRING[] = "vmet";
constexpr ui4 VIDEO_METADATA_FILE_TYPE_CODE = 0x74656d76;

constexpr char VIDEO_INDICES_FILE_TYPE_STRING[] = "vidx";
constexpr ui4 VIDEO_INDICES_FILE_TYPE_CODE = 0x78646976;

// ============================================================================
// Channel Types
// ============================================================================
constexpr si4 UNKNOWN_CHANNEL_TYPE = -1;
constexpr si4 TIME_SERIES_CHANNEL_TYPE = 1;
constexpr si4 VIDEO_CHANNEL_TYPE = 2;

// ============================================================================
// Universal Header Constants
// ============================================================================
constexpr si8 UNIVERSAL_HEADER_OFFSET = 0;
constexpr si4 UNIVERSAL_HEADER_BYTES = 1024;

constexpr si4 UNIVERSAL_HEADER_HEADER_CRC_OFFSET = 0;
constexpr si4 UNIVERSAL_HEADER_BODY_CRC_OFFSET = 4;
constexpr si4 UNIVERSAL_HEADER_FILE_TYPE_OFFSET = 8;
constexpr si4 UNIVERSAL_HEADER_MEF_VERSION_MAJOR_OFFSET = 13;
constexpr si4 UNIVERSAL_HEADER_MEF_VERSION_MINOR_OFFSET = 14;
constexpr si4 UNIVERSAL_HEADER_BYTE_ORDER_CODE_OFFSET = 15;
constexpr si4 UNIVERSAL_HEADER_START_TIME_OFFSET = 16;
constexpr si4 UNIVERSAL_HEADER_END_TIME_OFFSET = 24;
constexpr si4 UNIVERSAL_HEADER_NUMBER_OF_ENTRIES_OFFSET = 32;
constexpr si4 UNIVERSAL_HEADER_MAXIMUM_ENTRY_SIZE_OFFSET = 40;
constexpr si4 UNIVERSAL_HEADER_SEGMENT_NUMBER_OFFSET = 48;
constexpr si4 UNIVERSAL_HEADER_SEGMENT_NUMBER_NO_ENTRY = -1;
constexpr si4 UNIVERSAL_HEADER_CHANNEL_LEVEL_CODE = -2;
constexpr si4 UNIVERSAL_HEADER_SESSION_LEVEL_CODE = -3;
constexpr si4 UNIVERSAL_HEADER_CHANNEL_NAME_OFFSET = 52;
constexpr si4 UNIVERSAL_HEADER_SESSION_NAME_OFFSET = 308;
constexpr si4 UNIVERSAL_HEADER_ANONYMIZED_NAME_OFFSET = 564;
constexpr si4 UNIVERSAL_HEADER_ANONYMIZED_NAME_BYTES = 256;
constexpr si4 UNIVERSAL_HEADER_LEVEL_UUID_OFFSET = 820;
constexpr si4 UNIVERSAL_HEADER_FILE_UUID_OFFSET = 836;
constexpr si4 UNIVERSAL_HEADER_PROVENANCE_UUID_OFFSET = 852;
constexpr si4 UNIVERSAL_HEADER_LEVEL_1_PASSWORD_VALIDATION_FIELD_OFFSET = 868;
constexpr si4 UNIVERSAL_HEADER_LEVEL_2_PASSWORD_VALIDATION_FIELD_OFFSET = 884;
constexpr si4 UNIVERSAL_HEADER_PROTECTED_REGION_OFFSET = 900;
constexpr si4 UNIVERSAL_HEADER_PROTECTED_REGION_BYTES = 60;
constexpr si4 UNIVERSAL_HEADER_DISCRETIONARY_REGION_OFFSET = 960;
constexpr si4 UNIVERSAL_HEADER_DISCRETIONARY_REGION_BYTES = 64;

// ============================================================================
// Metadata Constants
// ============================================================================
constexpr si4 METADATA_FILE_BYTES = 16384;  // 16 KB

// Section 1
constexpr si4 METADATA_SECTION_1_BYTES = 1536;
constexpr si4 METADATA_SECTION_2_ENCRYPTION_OFFSET = 1024;
constexpr si1 METADATA_SECTION_2_ENCRYPTION_DEFAULT = LEVEL_1_ENCRYPTION;
constexpr si4 METADATA_SECTION_3_ENCRYPTION_OFFSET = 1025;
constexpr si1 METADATA_SECTION_3_ENCRYPTION_DEFAULT = LEVEL_2_ENCRYPTION;
constexpr si4 METADATA_SECTION_1_PROTECTED_REGION_OFFSET = 1026;
constexpr si4 METADATA_SECTION_1_PROTECTED_REGION_BYTES = 766;
constexpr si4 METADATA_SECTION_1_DISCRETIONARY_REGION_OFFSET = 1792;
constexpr si4 METADATA_SECTION_1_DISCRETIONARY_REGION_BYTES = 768;

// Section 2
constexpr si4 METADATA_SECTION_2_OFFSET = 2560;
constexpr si4 METADATA_SECTION_2_BYTES = 10752;
constexpr si4 METADATA_CHANNEL_DESCRIPTION_OFFSET = 2560;
constexpr si4 METADATA_CHANNEL_DESCRIPTION_BYTES = 2048;
constexpr si4 METADATA_SESSION_DESCRIPTION_OFFSET = 4608;
constexpr si4 METADATA_SESSION_DESCRIPTION_BYTES = 2048;
constexpr si4 METADATA_RECORDING_DURATION_OFFSET = 6656;
constexpr si8 METADATA_RECORDING_DURATION_NO_ENTRY = -1;

// Time Series Section 2
constexpr si4 TIME_SERIES_METADATA_REFERENCE_DESCRIPTION_OFFSET = 6664;
constexpr si4 TIME_SERIES_METADATA_REFERENCE_DESCRIPTION_BYTES = METADATA_CHANNEL_DESCRIPTION_BYTES;
constexpr si4 TIME_SERIES_METADATA_ACQUISITION_CHANNEL_NUMBER_OFFSET = 8712;
constexpr si8 TIME_SERIES_METADATA_ACQUISITION_CHANNEL_NUMBER_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_SAMPLING_FREQUENCY_OFFSET = 8720;
constexpr sf8 TIME_SERIES_METADATA_SAMPLING_FREQUENCY_NO_ENTRY = -1.0;
constexpr si4 TIME_SERIES_METADATA_LOW_FREQUENCY_FILTER_SETTING_OFFSET = 8728;
constexpr sf8 TIME_SERIES_METADATA_LOW_FREQUENCY_FILTER_SETTING_NO_ENTRY = -1.0;
constexpr si4 TIME_SERIES_METADATA_HIGH_FREQUENCY_FILTER_SETTING_OFFSET = 8736;
constexpr sf8 TIME_SERIES_METADATA_HIGH_FREQUENCY_FILTER_SETTING_NO_ENTRY = -1.0;
constexpr si4 TIME_SERIES_METADATA_NOTCH_FILTER_FREQUENCY_SETTING_OFFSET = 8744;
constexpr sf8 TIME_SERIES_METADATA_NOTCH_FILTER_FREQUENCY_SETTING_NO_ENTRY = -1.0;
constexpr si4 TIME_SERIES_METADATA_AC_LINE_FREQUENCY_OFFSET = 8752;
constexpr sf8 TIME_SERIES_METADATA_AC_LINE_FREQUENCY_NO_ENTRY = -1.0;
constexpr si4 TIME_SERIES_METADATA_UNITS_CONVERSION_FACTOR_OFFSET = 8760;
constexpr sf8 TIME_SERIES_METADATA_UNITS_CONVERSION_FACTOR_NO_ENTRY = 0.0;
constexpr si4 TIME_SERIES_METADATA_UNITS_DESCRIPTION_OFFSET = 8768;
constexpr si4 TIME_SERIES_METADATA_UNITS_DESCRIPTION_BYTES = 128;
constexpr si4 TIME_SERIES_METADATA_START_SAMPLE_OFFSET = 8912;
constexpr si8 TIME_SERIES_METADATA_START_SAMPLE_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_NUMBER_OF_SAMPLES_OFFSET = 8920;
constexpr si8 TIME_SERIES_METADATA_NUMBER_OF_SAMPLES_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_NUMBER_OF_BLOCKS_OFFSET = 8928;
constexpr si8 TIME_SERIES_METADATA_NUMBER_OF_BLOCKS_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_MAXIMUM_BLOCK_BYTES_OFFSET = 8936;
constexpr si8 TIME_SERIES_METADATA_MAXIMUM_BLOCK_BYTES_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_MAXIMUM_BLOCK_SAMPLES_OFFSET = 8944;
constexpr ui4 TIME_SERIES_METADATA_MAXIMUM_BLOCK_SAMPLES_NO_ENTRY = 0xFFFFFFFF;
constexpr si4 TIME_SERIES_METADATA_MAXIMUM_DIFFERENCE_BYTES_OFFSET = 8948;
constexpr ui4 TIME_SERIES_METADATA_MAXIMUM_DIFFERENCE_BYTES_NO_ENTRY = 0xFFFFFFFF;
constexpr si4 TIME_SERIES_METADATA_BLOCK_INTERVAL_OFFSET = 8952;
constexpr si8 TIME_SERIES_METADATA_BLOCK_INTERVAL_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_NUMBER_OF_DISCONTINUITIES_OFFSET = 8960;
constexpr si8 TIME_SERIES_METADATA_NUMBER_OF_DISCONTINUITIES_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_MAXIMUM_CONTIGUOUS_BLOCKS_OFFSET = 8968;
constexpr si8 TIME_SERIES_METADATA_MAXIMUM_CONTIGUOUS_BLOCKS_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_MAXIMUM_CONTIGUOUS_BLOCK_BYTES_OFFSET = 8976;
constexpr si8 TIME_SERIES_METADATA_MAXIMUM_CONTIGUOUS_BLOCK_BYTES_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_MAXIMUM_CONTIGUOUS_SAMPLES_OFFSET = 8984;
constexpr si8 TIME_SERIES_METADATA_MAXIMUM_CONTIGUOUS_SAMPLES_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_METADATA_SECTION_2_PROTECTED_REGION_OFFSET = 8992;
constexpr si4 TIME_SERIES_METADATA_SECTION_2_PROTECTED_REGION_BYTES = 2160;
constexpr si4 TIME_SERIES_METADATA_SECTION_2_DISCRETIONARY_REGION_OFFSET = 11152;
constexpr si4 TIME_SERIES_METADATA_SECTION_2_DISCRETIONARY_REGION_BYTES = 2160;

// Section 3
constexpr si4 METADATA_SECTION_3_OFFSET = 13312;
constexpr si4 METADATA_SECTION_3_BYTES = 3072;
constexpr si4 METADATA_RECORDING_TIME_OFFSET_OFFSET = 13312;
constexpr si8 METADATA_RECORDING_TIME_OFFSET_NO_ENTRY = UUTC_NO_ENTRY;
constexpr si4 METADATA_DST_START_TIME_OFFSET = 13320;
constexpr si8 METADATA_DST_START_TIME_NO_ENTRY = UUTC_NO_ENTRY;
constexpr si4 METADATA_DST_END_TIME_OFFSET = 13328;
constexpr si8 METADATA_DST_END_TIME_NO_ENTRY = UUTC_NO_ENTRY;
constexpr si4 METADATA_GMT_OFFSET_OFFSET = 13336;
constexpr si4 GMT_OFFSET_NO_ENTRY = -86401;
constexpr si4 METADATA_SUBJECT_NAME_1_OFFSET = 13340;
constexpr si4 METADATA_SUBJECT_NAME_BYTES = 128;
constexpr si4 METADATA_SUBJECT_NAME_2_OFFSET = 13468;
constexpr si4 METADATA_SUBJECT_ID_OFFSET = 13596;
constexpr si4 METADATA_SUBJECT_ID_BYTES = 128;
constexpr si4 METADATA_RECORDING_LOCATION_OFFSET = 13724;
constexpr si4 METADATA_RECORDING_LOCATION_BYTES = 512;
constexpr si4 METADATA_SECTION_3_PROTECTED_REGION_OFFSET = 14236;
constexpr si4 METADATA_SECTION_3_PROTECTED_REGION_BYTES = 1124;
constexpr si4 METADATA_SECTION_3_DISCRETIONARY_REGION_OFFSET = 15360;
constexpr si4 METADATA_SECTION_3_DISCRETIONARY_REGION_BYTES = 1024;

// ============================================================================
// Time Series Index Constants
// ============================================================================
constexpr si4 TIME_SERIES_INDEX_BYTES = 56;
constexpr si4 TIME_SERIES_INDEX_FILE_OFFSET_OFFSET = 0;
constexpr si8 TIME_SERIES_INDEX_FILE_OFFSET_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_INDEX_START_TIME_OFFSET = 8;
constexpr si8 TIME_SERIES_INDEX_START_TIME_NO_ENTRY = UUTC_NO_ENTRY;
constexpr si4 TIME_SERIES_INDEX_START_SAMPLE_OFFSET = 16;
constexpr si8 TIME_SERIES_INDEX_START_SAMPLE_NO_ENTRY = -1;
constexpr si4 TIME_SERIES_INDEX_NUMBER_OF_SAMPLES_OFFSET = 24;
constexpr ui4 TIME_SERIES_INDEX_NUMBER_OF_SAMPLES_NO_ENTRY = 0xFFFFFFFF;
constexpr si4 TIME_SERIES_INDEX_BLOCK_BYTES_OFFSET = 28;
constexpr ui4 TIME_SERIES_INDEX_BLOCK_BYTES_NO_ENTRY = 0xFFFFFFFF;
constexpr si4 TIME_SERIES_INDEX_MAXIMUM_SAMPLE_VALUE_OFFSET = 32;
constexpr si4 TIME_SERIES_INDEX_MINIMUM_SAMPLE_VALUE_OFFSET = 36;
constexpr si4 TIME_SERIES_INDEX_PROTECTED_REGION_OFFSET = 40;
constexpr si4 TIME_SERIES_INDEX_PROTECTED_REGION_BYTES = 4;
constexpr si4 TIME_SERIES_INDEX_RED_BLOCK_FLAGS_OFFSET = 44;
constexpr si4 TIME_SERIES_INDEX_RED_BLOCK_PROTECTED_REGION_OFFSET = 45;
constexpr si4 RED_BLOCK_PROTECTED_REGION_BYTES = 3;
constexpr si4 TIME_SERIES_INDEX_RED_BLOCK_DISCRETIONARY_REGION_OFFSET = 48;
constexpr si4 RED_BLOCK_DISCRETIONARY_REGION_BYTES = 8;

// ============================================================================
// Record Header Constants
// ============================================================================
constexpr si4 RECORD_HEADER_BYTES = 24;
constexpr si4 RECORD_HEADER_RECORD_CRC_OFFSET = 0;
constexpr ui4 RECORD_HEADER_RECORD_CRC_NO_ENTRY = CRC_NO_ENTRY;
constexpr si4 RECORD_HEADER_TYPE_OFFSET = 4;
constexpr ui4 RECORD_HEADER_TYPE_NO_ENTRY = 0;
constexpr si4 RECORD_HEADER_VERSION_MAJOR_OFFSET = 9;
constexpr ui1 RECORD_HEADER_VERSION_MAJOR_NO_ENTRY = 0xFF;
constexpr si4 RECORD_HEADER_VERSION_MINOR_OFFSET = 10;
constexpr ui1 RECORD_HEADER_VERSION_MINOR_NO_ENTRY = 0xFF;
constexpr si4 RECORD_HEADER_ENCRYPTION_OFFSET = 11;
constexpr si4 RECORD_HEADER_BYTES_OFFSET = 12;
constexpr ui4 RECORD_HEADER_BYTES_NO_ENTRY = 0;
constexpr si4 RECORD_HEADER_TIME_OFFSET = 16;
constexpr si8 RECORD_HEADER_TIME_NO_ENTRY = UUTC_NO_ENTRY;

// ============================================================================
// Record Index Constants
// ============================================================================
constexpr si4 RECORD_INDEX_BYTES = 24;
constexpr si4 RECORD_INDEX_TYPE_OFFSET = 0;
constexpr ui4 RECORD_INDEX_TYPE_NO_ENTRY = 0;
constexpr si4 RECORD_INDEX_VERSION_MAJOR_OFFSET = 5;
constexpr ui1 RECORD_INDEX_VERSION_MAJOR_NO_ENTRY = 0xFF;
constexpr si4 RECORD_INDEX_VERSION_MINOR_OFFSET = 6;
constexpr ui1 RECORD_INDEX_VERSION_MINOR_NO_ENTRY = 0xFF;
constexpr si4 RECORD_INDEX_ENCRYPTION_OFFSET = 7;
constexpr si4 RECORD_INDEX_FILE_OFFSET_OFFSET = 8;
constexpr si8 RECORD_INDEX_FILE_OFFSET_NO_ENTRY = -1;
constexpr si4 RECORD_INDEX_TIME_OFFSET = 16;
constexpr si8 RECORD_INDEX_TIME_NO_ENTRY = UUTC_NO_ENTRY;

// ============================================================================
// RED Block Constants
// ============================================================================
constexpr si4 RED_BLOCK_HEADER_BYTES = 304;
constexpr si4 RED_BLOCK_COMPRESSED_DATA_OFFSET = RED_BLOCK_HEADER_BYTES;
constexpr si4 RED_BLOCK_BLOCK_CRC_OFFSET = 0;
constexpr si4 RED_BLOCK_FLAGS_OFFSET = 4;
constexpr si4 RED_BLOCK_DETREND_SLOPE_OFFSET = 16;
constexpr si4 RED_BLOCK_DETREND_INTERCEPT_OFFSET = 20;
constexpr si4 RED_BLOCK_SCALE_FACTOR_OFFSET = 24;
constexpr si4 RED_BLOCK_DIFFERENCE_BYTES_OFFSET = 28;
constexpr si4 RED_BLOCK_NUMBER_OF_SAMPLES_OFFSET = 32;
constexpr si4 RED_BLOCK_BLOCK_BYTES_OFFSET = 36;
constexpr si4 RED_BLOCK_START_TIME_OFFSET = 40;
constexpr si4 RED_BLOCK_STATISTICS_OFFSET = 48;
constexpr si4 RED_BLOCK_STATISTICS_BYTES = 256;

// RED Flag Masks
constexpr ui1 RED_DISCONTINUITY_MASK = 0x01;
constexpr ui1 RED_LEVEL_1_ENCRYPTION_MASK = 0x02;
constexpr ui1 RED_LEVEL_2_ENCRYPTION_MASK = 0x04;

// RED Reserved Values
constexpr si4 RED_NAN = static_cast<si4>(0x80000000);
constexpr si4 RED_NEGATIVE_INFINITY = static_cast<si4>(0x80000001);
constexpr si4 RED_POSITIVE_INFINITY = static_cast<si4>(0x7FFFFFFF);
constexpr si4 RED_MAXIMUM_SAMPLE_VALUE = static_cast<si4>(0x7FFFFFFE);
constexpr si4 RED_MINIMUM_SAMPLE_VALUE = static_cast<si4>(0x80000002);

// RED Range Encoding Constants
constexpr ui4 TOP_VALUE = 0x80000000;
constexpr ui4 TOP_VALUE_MINUS_1 = 0x7FFFFFFF;
constexpr ui4 CARRY_CHECK = 0x7F800000;
constexpr si4 SHIFT_BITS = 23;
constexpr si4 EXTRA_BITS = 7;
constexpr ui4 BOTTOM_VALUE = 0x800000;

// RED Compression Modes
constexpr ui1 RED_DECOMPRESSION = 0;
constexpr ui1 RED_LOSSLESS_COMPRESSION = 1;
constexpr ui1 RED_FIXED_SCALE_FACTOR = 2;
constexpr ui1 RED_FIXED_COMPRESSION_RATIO = 4;
constexpr ui1 RED_MEAN_RESIDUAL_RATIO = 8;

// RED Defaults
constexpr ui1 RED_COMPRESSION_MODE_DEFAULT = RED_LOSSLESS_COMPRESSION;
constexpr si1 RED_ENCRYPTION_LEVEL_DEFAULT = NO_ENCRYPTION;
constexpr sf4 RED_SCALE_FACTOR_DEFAULT = 1.0f;
constexpr sf4 RED_DETREND_SLOPE_DEFAULT = 0.0f;
constexpr sf4 RED_DETREND_INTERCEPT_DEFAULT = 0.0f;
constexpr si4 RED_MAXIMUM_ROUNDS_PER_BLOCK_DEFAULT = 20;
constexpr si4 RED_RETURN_LOSSY_DATA_DEFAULT = MEF_FALSE;
constexpr si4 RED_DETREND_DATA_DEFAULT = MEF_FALSE;
constexpr si4 RED_VALIDATE_BLOCK_CRC_DEFAULT = MEF_FALSE;
constexpr si4 RED_DISCONTINUITY_DEFAULT = MEF_TRUE;
constexpr si4 RED_RESET_DISCONTINUITY_DEFAULT = MEF_TRUE;
constexpr sf8 RED_GOAL_COMPRESSION_RATIO_DEFAULT = 0.05;
constexpr sf8 RED_GOAL_MEAN_RESIDUAL_RATIO_DEFAULT = 0.05;
constexpr sf8 RED_GOAL_TOLERANCE_DEFAULT = 0.005;
constexpr si4 RED_REQUIRE_NORMALITY_DEFAULT = MEF_TRUE;
constexpr sf8 RED_NORMAL_CORRELATION_DEFAULT = 0.5;

// RED macros as inline functions
inline constexpr si8 RED_MAX_DIFFERENCE_BYTES(si8 x) { return x * 5; }
inline constexpr si8 RED_MAX_COMPRESSED_BYTES(si8 x, si8 y) {
    return (RED_MAX_DIFFERENCE_BYTES(x) + RED_BLOCK_HEADER_BYTES + 7) * y;
}

// RED Normal CDF Table Constants
constexpr si4 RED_NORMAL_CDF_TABLE_ENTRIES = 61;
constexpr sf8 RED_SUM_NORMAL_CDF = 30.5;
constexpr sf8 RED_SUM_SQ_NORMAL_CDF = 24.864467406647070;

// ============================================================================
// Error Behavior Constants
// ============================================================================
constexpr ui4 USE_GLOBAL_BEHAVIOR = 0;
constexpr ui4 RESTORE_BEHAVIOR = 1;
constexpr ui4 EXIT_ON_FAIL = 2;
constexpr ui4 RETURN_ON_FAIL = 4;
constexpr ui4 SUPPRESS_ERROR_OUTPUT = 8;

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_CONSTANTS_HPP
