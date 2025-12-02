/**
 * @file mef_writer.hpp
 * @brief MEF 3.0 file writer
 * 
 * Provides high-level API for writing MEF 3.0 sessions, channels, and segments.
 */

#ifndef BRAINMAZE_MEFD_MEF_WRITER_HPP
#define BRAINMAZE_MEFD_MEF_WRITER_HPP

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
 * @brief MEF 3.0 Session Writer
 * 
 * High-level interface for writing MEF 3.0 files.
 * Creates files compatible with meflib, pymef, and mef-tools.
 */
class MefWriter {
public:
    /**
     * @brief Constructor - create or open a MEF session
     * @param path Path to .mefd session directory
     * @param overwrite If true, overwrite existing session. If false, append.
     * @param password1 Optional level 1 (write) password
     * @param password2 Optional level 2 (read) password
     */
    MefWriter(const std::string& path, 
              bool overwrite = true,
              const std::string& password1 = "",
              const std::string& password2 = "");
    
    /**
     * @brief Destructor - ensures all data is flushed
     */
    ~MefWriter();

    // Prevent copying
    MefWriter(const MefWriter&) = delete;
    MefWriter& operator=(const MefWriter&) = delete;

    // Allow moving
    MefWriter(MefWriter&&) noexcept;
    MefWriter& operator=(MefWriter&&) noexcept;

    /**
     * @brief Check if the writer is ready
     * @return true if writer is valid
     */
    bool is_valid() const { return m_valid; }

    // ========================================================================
    // Properties (Python-style interface)
    // ========================================================================

    /**
     * @brief Get/set MEF block length (samples per block)
     */
    si4 get_mef_block_len() const { return m_block_len; }
    void set_mef_block_len(si4 value) { m_block_len = value; }

    /**
     * @brief Get/set maximum NaN samples to write
     */
    si4 get_max_nans_written() const { return m_max_nans; }
    void set_max_nans_written(si4 value) { m_max_nans = value; }

    /**
     * @brief Get/set data units description
     */
    const std::string& get_data_units() const { return m_data_units; }
    void set_data_units(const std::string& value) { m_data_units = value; }

    /**
     * @brief Get/set units conversion factor
     */
    sf8 get_units_conversion_factor() const { return m_units_conversion_factor; }
    void set_units_conversion_factor(sf8 value) { m_units_conversion_factor = value; }

    /**
     * @brief Get/set recording time offset
     */
    si8 get_recording_time_offset() const { return m_recording_time_offset; }
    void set_recording_time_offset(si8 value) { m_recording_time_offset = value; }

    /**
     * @brief Get/set GMT offset
     */
    si4 get_gmt_offset() const { return m_gmt_offset; }
    void set_gmt_offset(si4 value) { m_gmt_offset = value; }

    /**
     * @brief Get/set subject name
     */
    const std::string& get_subject_name() const { return m_subject_name; }
    void set_subject_name(const std::string& value) { m_subject_name = value; }

    /**
     * @brief Get/set subject ID
     */
    const std::string& get_subject_id() const { return m_subject_id; }
    void set_subject_id(const std::string& value) { m_subject_id = value; }

    /**
     * @brief Get/set recording location
     */
    const std::string& get_recording_location() const { return m_recording_location; }
    void set_recording_location(const std::string& value) { m_recording_location = value; }

    /**
     * @brief Get/set channel description
     */
    const std::string& get_channel_description() const { return m_channel_description; }
    void set_channel_description(const std::string& value) { m_channel_description = value; }

    /**
     * @brief Get/set session description
     */
    const std::string& get_session_description() const { return m_session_description; }
    void set_session_description(const std::string& value) { m_session_description = value; }

    // ========================================================================
    // Writing Methods
    // ========================================================================

    /**
     * @brief Write data to a channel
     * @param data Sample data (float64)
     * @param channel_name Name of the channel
     * @param start_uutc Start time in microseconds since epoch
     * @param sampling_freq Sampling frequency in Hz
     * @param precision Optional decimal precision for conversion
     * @param new_segment If true, create a new segment
     */
    void write_data(const std::vector<sf8>& data,
                    const std::string& channel_name,
                    si8 start_uutc,
                    sf8 sampling_freq,
                    si4 precision = -1,
                    bool new_segment = false);

    /**
     * @brief Write data to a channel (raw pointer version)
     * @param data Pointer to sample data (float64)
     * @param num_samples Number of samples
     * @param channel_name Name of the channel
     * @param start_uutc Start time in microseconds since epoch
     * @param sampling_freq Sampling frequency in Hz
     * @param precision Optional decimal precision for conversion
     * @param new_segment If true, create a new segment
     */
    void write_data(const sf8* data,
                    size_t num_samples,
                    const std::string& channel_name,
                    si8 start_uutc,
                    sf8 sampling_freq,
                    si4 precision = -1,
                    bool new_segment = false);

    /**
     * @brief Write raw integer data to a channel
     * @param data Sample data (si4)
     * @param channel_name Name of the channel
     * @param start_uutc Start time in microseconds since epoch
     * @param sampling_freq Sampling frequency in Hz
     * @param new_segment If true, create a new segment
     */
    void write_raw_data(const std::vector<si4>& data,
                        const std::string& channel_name,
                        si8 start_uutc,
                        sf8 sampling_freq,
                        bool new_segment = false);

    /**
     * @brief Flush and finalize all data
     * 
     * This is called automatically by the destructor, but can be called
     * explicitly to ensure data is written to disk.
     */
    void flush();

    /**
     * @brief Close the session
     * 
     * After calling close(), no more data can be written.
     */
    void close();

private:
    // Internal implementation
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    // Session state
    bool m_valid = false;
    bool m_closed = false;
    std::string m_path;
    std::string m_session_name;
    bool m_overwrite = true;
    std::string m_password1;
    std::string m_password2;

    // Configuration
    si4 m_block_len = 1000;
    si4 m_max_nans = 0;
    std::string m_data_units = "V";
    sf8 m_units_conversion_factor = 1.0;
    si8 m_recording_time_offset = 0;
    si4 m_gmt_offset = GMT_OFFSET_NO_ENTRY;
    std::string m_subject_name;
    std::string m_subject_id;
    std::string m_recording_location;
    std::string m_channel_description;
    std::string m_session_description;

    // Channel tracking
    struct ChannelState {
        std::filesystem::path path;
        si4 current_segment = -1;
        si8 last_sample_index = 0;
        si8 last_end_time = UUTC_NO_ENTRY;
        sf8 sampling_frequency = 0.0;
        std::vector<TimeSeriesIndex> indices;
        si8 total_samples = 0;
        si4 total_blocks = 0;
    };
    std::map<std::string, ChannelState> m_channel_states;

    // Internal methods
    bool create_session();
    void ensure_channel(const std::string& channel_name, sf8 sampling_freq);
    void create_segment(const std::string& channel_name);
    void write_block(const std::string& channel_name, 
                     const si4* samples, ui4 num_samples, si8 start_time,
                     bool is_discontinuity);
    void finalize_channel(const std::string& channel_name);
    void finalize_segment(const std::string& channel_name, si4 segment_num);
    void write_metadata(const std::string& channel_name, si4 segment_num);
    void write_indices(const std::string& channel_name, si4 segment_num);
};

} // namespace brainmaze_mefd

#endif // BRAINMAZE_MEFD_MEF_WRITER_HPP
