/**
 * @file mef_reader.hpp
 * @brief MEF 3.0 file reader
 * 
 * Provides high-level API for reading MEF 3.0 sessions, channels, and segments.
 */

#ifndef BRAINMAZE_MEFD_MEF_READER_HPP
#define BRAINMAZE_MEFD_MEF_READER_HPP

#include "types.hpp"
#include "constants.hpp"
#include "structures.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>

namespace brainmaze_mefd {

/**
 * @brief MEF 3.0 Session Reader
 * 
 * High-level interface for reading MEF 3.0 files.
 * Maintains full compatibility with files created by meflib, pymef, and mef-tools.
 */
class MefReader {
public:
    /**
     * @brief Channel information structure
     */
    struct ChannelInfo {
        std::string name;
        si4 channel_type = TIME_SERIES_CHANNEL_TYPE;
        sf8 sampling_frequency = 0.0;
        si8 number_of_samples = 0;
        si8 start_time = UUTC_NO_ENTRY;
        si8 end_time = UUTC_NO_ENTRY;
        std::string units;
        sf8 units_conversion_factor = 1.0;
        si4 number_of_segments = 0;
    };

    /**
     * @brief Segment information structure
     */
    struct SegmentInfo {
        std::string name;
        si4 segment_number = 0;
        si8 start_time = UUTC_NO_ENTRY;
        si8 end_time = UUTC_NO_ENTRY;
        si8 start_sample = 0;
        si8 number_of_samples = 0;
        si8 number_of_blocks = 0;
    };

    /**
     * @brief Constructor - open a MEF session
     * @param path Path to .mefd session directory
     * @param password Optional password for encrypted files
     */
    explicit MefReader(const std::string& path, const std::string& password = "");
    
    /**
     * @brief Destructor
     */
    ~MefReader();

    // Prevent copying
    MefReader(const MefReader&) = delete;
    MefReader& operator=(const MefReader&) = delete;

    // Allow moving
    MefReader(MefReader&&) noexcept;
    MefReader& operator=(MefReader&&) noexcept;

    /**
     * @brief Check if the session was loaded successfully
     * @return true if session is valid
     */
    bool is_valid() const { return m_valid; }

    /**
     * @brief Get the session path
     * @return Path to the .mefd directory
     */
    const std::string& get_path() const { return m_path; }

    /**
     * @brief Get session name
     * @return Session name
     */
    const std::string& get_session_name() const { return m_session_name; }

    /**
     * @brief Get list of channel names
     * @return Vector of channel names
     */
    std::vector<std::string> get_channels() const;

    /**
     * @brief Get list of time series channel names
     * @return Vector of time series channel names
     */
    std::vector<std::string> get_time_series_channels() const;

    /**
     * @brief Get channel information
     * @param channel_name Name of the channel
     * @return ChannelInfo structure
     * @throws std::runtime_error if channel not found
     */
    ChannelInfo get_channel_info(const std::string& channel_name) const;

    /**
     * @brief Get segment information for a channel
     * @param channel_name Name of the channel
     * @return Vector of SegmentInfo structures
     */
    std::vector<SegmentInfo> get_segments(const std::string& channel_name) const;

    /**
     * @brief Get a property value
     * @param property_name Name of the property
     * @param channel_name Optional channel name (nullptr for session-level)
     * @return Property value as a variant type (string or double)
     */
    sf8 get_numeric_property(const std::string& property_name, 
                             const std::string& channel_name = "") const;
    
    std::string get_string_property(const std::string& property_name,
                                    const std::string& channel_name = "") const;

    /**
     * @brief Read data from a channel
     * @param channel_name Name of the channel
     * @param start_time Start time in uUTC (optional, nullptr = beginning)
     * @param end_time End time in uUTC (optional, nullptr = end)
     * @return Vector of samples (float64)
     */
    std::vector<sf8> get_data(const std::string& channel_name,
                              const si8* start_time = nullptr,
                              const si8* end_time = nullptr) const;

    /**
     * @brief Read raw samples from a channel
     * @param channel_name Name of the channel
     * @param start_sample Start sample index
     * @param end_sample End sample index (exclusive)
     * @return Vector of raw samples (si4)
     */
    std::vector<si4> get_raw_data(const std::string& channel_name,
                                  si8 start_sample,
                                  si8 end_sample) const;

    /**
     * @brief Get session start time
     * @return Start time in uUTC
     */
    si8 get_start_time() const { return m_start_time; }

    /**
     * @brief Get session end time
     * @return End time in uUTC
     */
    si8 get_end_time() const { return m_end_time; }

    /**
     * @brief Get recording duration in microseconds
     * @return Duration in microseconds
     */
    si8 get_duration() const { return m_end_time - m_start_time; }

private:
    // Internal implementation
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    // Session state
    bool m_valid = false;
    std::string m_path;
    std::string m_session_name;
    std::string m_password;
    si8 m_start_time = UUTC_NO_ENTRY;
    si8 m_end_time = UUTC_NO_ENTRY;

    // Channel data
    std::map<std::string, ChannelInfo> m_channels;

    // Internal methods
    bool load_session();
    bool load_channel(const std::filesystem::path& channel_path);
    bool load_segment(const std::filesystem::path& segment_path, 
                      const std::string& channel_name);
    std::vector<TimeSeriesIndex> read_indices(const std::filesystem::path& indices_path) const;
    std::vector<si4> decompress_blocks(const std::filesystem::path& data_path,
                                       const std::vector<TimeSeriesIndex>& indices,
                                       si8 start_idx, si8 end_idx) const;
};

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_MEF_READER_HPP
