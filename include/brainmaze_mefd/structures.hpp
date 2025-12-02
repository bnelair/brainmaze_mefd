/**
 * @file structures.hpp
 * @brief MEF 3.0 data structures
 * 
 * Defines all MEF 3.0 structures including:
 * - Universal Header
 * - Metadata (Sections 1, 2, 3)
 * - Time Series Index
 * - Record structures
 * - RED Block Header
 */

#ifndef BRAINMAZE_MEFD_STRUCTURES_HPP
#define BRAINMAZE_MEFD_STRUCTURES_HPP

#include "types.hpp"
#include "constants.hpp"
#include <array>
#include <string>
#include <vector>
#include <cstring>
#include <limits>

namespace brainmaze_mefd {

// Ensure structures are packed for binary compatibility
#pragma pack(push, 1)

/**
 * @brief Password data structure
 */
struct PasswordData {
    std::array<ui1, ENCRYPTION_KEY_BYTES> level_1_encryption_key;
    std::array<ui1, ENCRYPTION_KEY_BYTES> level_2_encryption_key;
    ui1 access_level;
    
    PasswordData() : access_level(0) {
        level_1_encryption_key.fill(0);
        level_2_encryption_key.fill(0);
    }
};

/**
 * @brief Universal Header structure (1024 bytes)
 * 
 * Present at the beginning of every MEF 3.0 file.
 */
struct UniversalHeader {
    ui4 header_CRC;
    ui4 body_CRC;
    std::array<si1, TYPE_BYTES> file_type_string;
    ui1 mef_version_major;
    ui1 mef_version_minor;
    ui1 byte_order_code;
    si8 start_time;
    si8 end_time;
    si8 number_of_entries;
    si8 maximum_entry_size;
    si4 segment_number;
    std::array<si1, MEF_BASE_FILE_NAME_BYTES> channel_name;
    std::array<si1, MEF_BASE_FILE_NAME_BYTES> session_name;
    std::array<si1, UNIVERSAL_HEADER_ANONYMIZED_NAME_BYTES> anonymized_name;
    std::array<ui1, UUID_BYTES> level_UUID;
    std::array<ui1, UUID_BYTES> file_UUID;
    std::array<ui1, UUID_BYTES> provenance_UUID;
    std::array<ui1, PASSWORD_VALIDATION_FIELD_BYTES> level_1_password_validation_field;
    std::array<ui1, PASSWORD_VALIDATION_FIELD_BYTES> level_2_password_validation_field;
    std::array<ui1, UNIVERSAL_HEADER_PROTECTED_REGION_BYTES> protected_region;
    std::array<ui1, UNIVERSAL_HEADER_DISCRETIONARY_REGION_BYTES> discretionary_region;
    
    UniversalHeader() { clear(); }
    
    void clear() {
        header_CRC = CRC_NO_ENTRY;
        body_CRC = CRC_NO_ENTRY;
        file_type_string.fill(0);
        mef_version_major = MEF_VERSION_MAJOR;
        mef_version_minor = MEF_VERSION_MINOR;
        byte_order_code = cpu_endianness();
        start_time = UUTC_NO_ENTRY;
        end_time = UUTC_NO_ENTRY;
        number_of_entries = UNKNOWN_NUMBER_OF_ENTRIES;
        maximum_entry_size = UNKNOWN_NUMBER_OF_ENTRIES;
        segment_number = UNIVERSAL_HEADER_SEGMENT_NUMBER_NO_ENTRY;
        channel_name.fill(0);
        session_name.fill(0);
        anonymized_name.fill(0);
        level_UUID.fill(0);
        file_UUID.fill(0);
        provenance_UUID.fill(0);
        level_1_password_validation_field.fill(0);
        level_2_password_validation_field.fill(0);
        protected_region.fill(PAD_BYTE_VALUE);
        discretionary_region.fill(PAD_BYTE_VALUE);
    }
    
    std::string get_file_type() const {
        return std::string(file_type_string.data(), std::strlen(file_type_string.data()));
    }
    
    void set_file_type(const std::string& type) {
        file_type_string.fill(0);
        std::strncpy(file_type_string.data(), type.c_str(), TYPE_BYTES - 1);
    }
    
    std::string get_channel_name() const {
        return std::string(channel_name.data(), std::strlen(channel_name.data()));
    }
    
    void set_channel_name(const std::string& name) {
        channel_name.fill(0);
        std::strncpy(channel_name.data(), name.c_str(), MEF_BASE_FILE_NAME_BYTES - 1);
    }
    
    std::string get_session_name() const {
        return std::string(session_name.data(), std::strlen(session_name.data()));
    }
    
    void set_session_name(const std::string& name) {
        session_name.fill(0);
        std::strncpy(session_name.data(), name.c_str(), MEF_BASE_FILE_NAME_BYTES - 1);
    }
};

/**
 * @brief Metadata Section 1 structure
 */
struct MetadataSection1 {
    si1 section_2_encryption;
    si1 section_3_encryption;
    std::array<ui1, METADATA_SECTION_1_PROTECTED_REGION_BYTES> protected_region;
    std::array<ui1, METADATA_SECTION_1_DISCRETIONARY_REGION_BYTES> discretionary_region;
    
    MetadataSection1() { clear(); }
    
    void clear() {
        section_2_encryption = METADATA_SECTION_2_ENCRYPTION_DEFAULT;
        section_3_encryption = METADATA_SECTION_3_ENCRYPTION_DEFAULT;
        protected_region.fill(PAD_BYTE_VALUE);
        discretionary_region.fill(PAD_BYTE_VALUE);
    }
};

/**
 * @brief Time Series Metadata Section 2 structure
 */
struct TimeSeriesMetadataSection2 {
    // Type-independent fields
    std::array<si1, METADATA_CHANNEL_DESCRIPTION_BYTES> channel_description;
    std::array<si1, METADATA_SESSION_DESCRIPTION_BYTES> session_description;
    si8 recording_duration;
    
    // Type-specific fields
    std::array<si1, TIME_SERIES_METADATA_REFERENCE_DESCRIPTION_BYTES> reference_description;
    si8 acquisition_channel_number;
    sf8 sampling_frequency;
    sf8 low_frequency_filter_setting;
    sf8 high_frequency_filter_setting;
    sf8 notch_filter_frequency_setting;
    sf8 AC_line_frequency;
    sf8 units_conversion_factor;
    std::array<si1, TIME_SERIES_METADATA_UNITS_DESCRIPTION_BYTES> units_description;
    sf8 maximum_native_sample_value;
    sf8 minimum_native_sample_value;
    si8 start_sample;
    si8 number_of_samples;
    si8 number_of_blocks;
    si8 maximum_block_bytes;
    ui4 maximum_block_samples;
    ui4 maximum_difference_bytes;
    si8 block_interval;
    si8 number_of_discontinuities;
    si8 maximum_contiguous_blocks;
    si8 maximum_contiguous_block_bytes;
    si8 maximum_contiguous_samples;
    std::array<ui1, TIME_SERIES_METADATA_SECTION_2_PROTECTED_REGION_BYTES> protected_region;
    std::array<ui1, TIME_SERIES_METADATA_SECTION_2_DISCRETIONARY_REGION_BYTES> discretionary_region;
    
    TimeSeriesMetadataSection2() { clear(); }
    
    void clear() {
        channel_description.fill(0);
        session_description.fill(0);
        recording_duration = METADATA_RECORDING_DURATION_NO_ENTRY;
        reference_description.fill(0);
        acquisition_channel_number = TIME_SERIES_METADATA_ACQUISITION_CHANNEL_NUMBER_NO_ENTRY;
        sampling_frequency = TIME_SERIES_METADATA_SAMPLING_FREQUENCY_NO_ENTRY;
        low_frequency_filter_setting = TIME_SERIES_METADATA_LOW_FREQUENCY_FILTER_SETTING_NO_ENTRY;
        high_frequency_filter_setting = TIME_SERIES_METADATA_HIGH_FREQUENCY_FILTER_SETTING_NO_ENTRY;
        notch_filter_frequency_setting = TIME_SERIES_METADATA_NOTCH_FILTER_FREQUENCY_SETTING_NO_ENTRY;
        AC_line_frequency = TIME_SERIES_METADATA_AC_LINE_FREQUENCY_NO_ENTRY;
        units_conversion_factor = TIME_SERIES_METADATA_UNITS_CONVERSION_FACTOR_NO_ENTRY;
        units_description.fill(0);
        maximum_native_sample_value = std::numeric_limits<sf8>::quiet_NaN();
        minimum_native_sample_value = std::numeric_limits<sf8>::quiet_NaN();
        start_sample = TIME_SERIES_METADATA_START_SAMPLE_NO_ENTRY;
        number_of_samples = TIME_SERIES_METADATA_NUMBER_OF_SAMPLES_NO_ENTRY;
        number_of_blocks = TIME_SERIES_METADATA_NUMBER_OF_BLOCKS_NO_ENTRY;
        maximum_block_bytes = TIME_SERIES_METADATA_MAXIMUM_BLOCK_BYTES_NO_ENTRY;
        maximum_block_samples = TIME_SERIES_METADATA_MAXIMUM_BLOCK_SAMPLES_NO_ENTRY;
        maximum_difference_bytes = TIME_SERIES_METADATA_MAXIMUM_DIFFERENCE_BYTES_NO_ENTRY;
        block_interval = TIME_SERIES_METADATA_BLOCK_INTERVAL_NO_ENTRY;
        number_of_discontinuities = TIME_SERIES_METADATA_NUMBER_OF_DISCONTINUITIES_NO_ENTRY;
        maximum_contiguous_blocks = TIME_SERIES_METADATA_MAXIMUM_CONTIGUOUS_BLOCKS_NO_ENTRY;
        maximum_contiguous_block_bytes = TIME_SERIES_METADATA_MAXIMUM_CONTIGUOUS_BLOCK_BYTES_NO_ENTRY;
        maximum_contiguous_samples = TIME_SERIES_METADATA_MAXIMUM_CONTIGUOUS_SAMPLES_NO_ENTRY;
        protected_region.fill(PAD_BYTE_VALUE);
        discretionary_region.fill(PAD_BYTE_VALUE);
    }
    
    void set_units_description(const std::string& units) {
        units_description.fill(0);
        std::strncpy(units_description.data(), units.c_str(), 
                     TIME_SERIES_METADATA_UNITS_DESCRIPTION_BYTES - 1);
    }
    
    std::string get_units_description() const {
        return std::string(units_description.data(), std::strlen(units_description.data()));
    }
};

/**
 * @brief Metadata Section 3 structure
 */
struct MetadataSection3 {
    si8 recording_time_offset;
    si8 DST_start_time;
    si8 DST_end_time;
    si4 GMT_offset;
    std::array<si1, METADATA_SUBJECT_NAME_BYTES> subject_name_1;
    std::array<si1, METADATA_SUBJECT_NAME_BYTES> subject_name_2;
    std::array<si1, METADATA_SUBJECT_ID_BYTES> subject_ID;
    std::array<si1, METADATA_RECORDING_LOCATION_BYTES> recording_location;
    std::array<ui1, METADATA_SECTION_3_PROTECTED_REGION_BYTES> protected_region;
    std::array<ui1, METADATA_SECTION_3_DISCRETIONARY_REGION_BYTES> discretionary_region;
    
    MetadataSection3() { clear(); }
    
    void clear() {
        recording_time_offset = METADATA_RECORDING_TIME_OFFSET_NO_ENTRY;
        DST_start_time = METADATA_DST_START_TIME_NO_ENTRY;
        DST_end_time = METADATA_DST_END_TIME_NO_ENTRY;
        GMT_offset = GMT_OFFSET_NO_ENTRY;
        subject_name_1.fill(0);
        subject_name_2.fill(0);
        subject_ID.fill(0);
        recording_location.fill(0);
        protected_region.fill(PAD_BYTE_VALUE);
        discretionary_region.fill(PAD_BYTE_VALUE);
    }
};

/**
 * @brief Time Series Index structure (56 bytes)
 */
struct TimeSeriesIndex {
    si8 file_offset;
    si8 start_time;
    si8 start_sample;
    ui4 number_of_samples;
    ui4 block_bytes;
    si4 maximum_sample_value;
    si4 minimum_sample_value;
    std::array<ui1, TIME_SERIES_INDEX_PROTECTED_REGION_BYTES> protected_region;
    ui1 RED_block_flags;
    std::array<ui1, RED_BLOCK_PROTECTED_REGION_BYTES> RED_block_protected_region;
    std::array<ui1, RED_BLOCK_DISCRETIONARY_REGION_BYTES> RED_block_discretionary_region;
    
    TimeSeriesIndex() { clear(); }
    
    void clear() {
        file_offset = TIME_SERIES_INDEX_FILE_OFFSET_NO_ENTRY;
        start_time = TIME_SERIES_INDEX_START_TIME_NO_ENTRY;
        start_sample = TIME_SERIES_INDEX_START_SAMPLE_NO_ENTRY;
        number_of_samples = TIME_SERIES_INDEX_NUMBER_OF_SAMPLES_NO_ENTRY;
        block_bytes = TIME_SERIES_INDEX_BLOCK_BYTES_NO_ENTRY;
        maximum_sample_value = RED_NAN;
        minimum_sample_value = RED_NAN;
        protected_region.fill(PAD_BYTE_VALUE);
        RED_block_flags = 0;
        RED_block_protected_region.fill(PAD_BYTE_VALUE);
        RED_block_discretionary_region.fill(PAD_BYTE_VALUE);
    }
};

/**
 * @brief Record Header structure (24 bytes)
 */
struct RecordHeader {
    ui4 record_CRC;
    std::array<si1, TYPE_BYTES> type_string;
    ui1 version_major;
    ui1 version_minor;
    si1 encryption;
    ui4 bytes;
    si8 time;
    
    RecordHeader() { clear(); }
    
    void clear() {
        record_CRC = RECORD_HEADER_RECORD_CRC_NO_ENTRY;
        type_string.fill(0);
        version_major = RECORD_HEADER_VERSION_MAJOR_NO_ENTRY;
        version_minor = RECORD_HEADER_VERSION_MINOR_NO_ENTRY;
        encryption = NO_ENCRYPTION;
        bytes = RECORD_HEADER_BYTES_NO_ENTRY;
        time = RECORD_HEADER_TIME_NO_ENTRY;
    }
    
    std::string get_type() const {
        return std::string(type_string.data(), std::strlen(type_string.data()));
    }
    
    void set_type(const std::string& type) {
        type_string.fill(0);
        std::strncpy(type_string.data(), type.c_str(), TYPE_BYTES - 1);
    }
};

/**
 * @brief Record Index structure (24 bytes)
 */
struct RecordIndex {
    std::array<si1, TYPE_BYTES> type_string;
    ui1 version_major;
    ui1 version_minor;
    si1 encryption;
    si8 file_offset;
    si8 time;
    
    RecordIndex() { clear(); }
    
    void clear() {
        type_string.fill(0);
        version_major = RECORD_INDEX_VERSION_MAJOR_NO_ENTRY;
        version_minor = RECORD_INDEX_VERSION_MINOR_NO_ENTRY;
        encryption = NO_ENCRYPTION;
        file_offset = RECORD_INDEX_FILE_OFFSET_NO_ENTRY;
        time = RECORD_INDEX_TIME_NO_ENTRY;
    }
};

/**
 * @brief RED Block Header structure (304 bytes)
 */
struct REDBlockHeader {
    ui4 block_CRC;
    ui1 flags;
    std::array<ui1, RED_BLOCK_PROTECTED_REGION_BYTES> protected_region;
    std::array<ui1, RED_BLOCK_DISCRETIONARY_REGION_BYTES> discretionary_region;
    sf4 detrend_slope;
    sf4 detrend_intercept;
    sf4 scale_factor;
    ui4 difference_bytes;
    ui4 number_of_samples;
    ui4 block_bytes;
    si8 start_time;
    std::array<ui1, RED_BLOCK_STATISTICS_BYTES> statistics;
    
    REDBlockHeader() { clear(); }
    
    void clear() {
        block_CRC = CRC_NO_ENTRY;
        flags = 0;
        protected_region.fill(PAD_BYTE_VALUE);
        discretionary_region.fill(PAD_BYTE_VALUE);
        detrend_slope = RED_DETREND_SLOPE_DEFAULT;
        detrend_intercept = RED_DETREND_INTERCEPT_DEFAULT;
        scale_factor = RED_SCALE_FACTOR_DEFAULT;
        difference_bytes = 0;
        number_of_samples = 0;
        block_bytes = 0;
        start_time = UUTC_NO_ENTRY;
        statistics.fill(0);
    }
    
    bool is_discontinuity() const { return (flags & RED_DISCONTINUITY_MASK) != 0; }
    bool is_level_1_encrypted() const { return (flags & RED_LEVEL_1_ENCRYPTION_MASK) != 0; }
    bool is_level_2_encrypted() const { return (flags & RED_LEVEL_2_ENCRYPTION_MASK) != 0; }
    
    void set_discontinuity(bool value) {
        if (value) flags |= RED_DISCONTINUITY_MASK;
        else flags &= ~RED_DISCONTINUITY_MASK;
    }
    
    void set_level_1_encryption(bool value) {
        if (value) flags |= RED_LEVEL_1_ENCRYPTION_MASK;
        else flags &= ~RED_LEVEL_1_ENCRYPTION_MASK;
    }
    
    void set_level_2_encryption(bool value) {
        if (value) flags |= RED_LEVEL_2_ENCRYPTION_MASK;
        else flags &= ~RED_LEVEL_2_ENCRYPTION_MASK;
    }
};

#pragma pack(pop)

// Static assertions to ensure structure sizes match MEF format
static_assert(sizeof(UniversalHeader) == UNIVERSAL_HEADER_BYTES,
              "UniversalHeader size mismatch");
static_assert(sizeof(TimeSeriesIndex) == TIME_SERIES_INDEX_BYTES,
              "TimeSeriesIndex size mismatch");
static_assert(sizeof(RecordHeader) == RECORD_HEADER_BYTES,
              "RecordHeader size mismatch");
static_assert(sizeof(RecordIndex) == RECORD_INDEX_BYTES,
              "RecordIndex size mismatch");
static_assert(sizeof(REDBlockHeader) == RED_BLOCK_HEADER_BYTES,
              "REDBlockHeader size mismatch");

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_STRUCTURES_HPP
