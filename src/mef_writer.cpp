/**
 * @file mef_writer.cpp
 * @brief MEF 3.0 file writer implementation
 */

#include "brainmaze_mefd/mef_writer.hpp"
#include "brainmaze_mefd/red.hpp"
#include "brainmaze_mefd/crc.hpp"
#include "brainmaze_mefd/aes.hpp"
#include "brainmaze_mefd/sha256.hpp"
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <cmath>
#include <random>
#include <chrono>

namespace brainmaze_mefd {

namespace fs = std::filesystem;

// Internal implementation details
struct MefWriter::Impl {
    PasswordData password_data;
    std::array<ui1, UUID_BYTES> session_uuid;
    
    std::map<std::string, std::ofstream> data_files;
    std::map<std::string, si8> data_file_offsets;
    
    // Generate a random UUID
    static void generate_uuid(std::array<ui1, UUID_BYTES>& uuid) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(0, 255);
        
        for (auto& byte : uuid) {
            byte = static_cast<ui1>(dis(gen));
        }
        
        // Set version (4) and variant bits per RFC 4122
        uuid[6] = (uuid[6] & 0x0F) | 0x40;  // Version 4
        uuid[8] = (uuid[8] & 0x3F) | 0x80;  // Variant 1
    }
};

MefWriter::MefWriter(const std::string& path, bool overwrite,
                     const std::string& password1, const std::string& password2)
    : m_impl(std::make_unique<Impl>())
    , m_path(path)
    , m_overwrite(overwrite)
    , m_password1(password1)
    , m_password2(password2)
{
    // Generate session UUID
    Impl::generate_uuid(m_impl->session_uuid);
    
    // Extract session name from path
    fs::path session_path(m_path);
    m_session_name = session_path.stem().string();
    if (m_session_name.ends_with(".mefd")) {
        m_session_name = m_session_name.substr(0, m_session_name.length() - 5);
    }
    
    // Setup password data if passwords provided
    if (!password1.empty()) {
        AES128::key_expansion(password1.c_str(), 
                              m_impl->password_data.level_1_encryption_key.data());
        m_impl->password_data.access_level = LEVEL_1_ACCESS;
    }
    if (!password2.empty()) {
        AES128::key_expansion(password2.c_str(), 
                              m_impl->password_data.level_2_encryption_key.data());
        if (m_impl->password_data.access_level < LEVEL_2_ACCESS) {
            m_impl->password_data.access_level = LEVEL_2_ACCESS;
        }
    }
    
    m_valid = create_session();
}

MefWriter::~MefWriter() {
    if (m_valid && !m_closed) {
        try {
            close();
        } catch (...) {
            // Ignore exceptions in destructor
        }
    }
}

MefWriter::MefWriter(MefWriter&& other) noexcept = default;
MefWriter& MefWriter::operator=(MefWriter&& other) noexcept = default;

bool MefWriter::create_session() {
    fs::path session_path(m_path);
    
    // Ensure path ends with .mefd extension
    if (session_path.extension() != ".mefd") {
        session_path.replace_extension(".mefd");
        m_path = session_path.string();
    }
    
    // Handle existing session
    if (fs::exists(session_path)) {
        if (m_overwrite) {
            fs::remove_all(session_path);
        }
        // If not overwriting, we'll append to existing
    }
    
    // Create session directory
    try {
        fs::create_directories(session_path);
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}

void MefWriter::ensure_channel(const std::string& channel_name, sf8 sampling_freq) {
    if (m_channel_states.count(channel_name)) {
        // Channel already exists, verify sampling frequency matches
        auto& state = m_channel_states[channel_name];
        if (state.sampling_frequency != 0.0 && state.sampling_frequency != sampling_freq) {
            throw std::runtime_error("Sampling frequency mismatch for channel: " + channel_name);
        }
        return;
    }
    
    // Create new channel
    fs::path channel_path = fs::path(m_path) / (channel_name + ".timd");
    fs::create_directories(channel_path);
    
    ChannelState state;
    state.path = channel_path;
    state.sampling_frequency = sampling_freq;
    state.current_segment = -1;  // No segment created yet
    
    m_channel_states[channel_name] = state;
}

void MefWriter::create_segment(const std::string& channel_name) {
    auto& state = m_channel_states[channel_name];
    state.current_segment++;
    
    // Format segment number with leading zeros (buffer sized for max int value)
    char seg_num_str[16];
    snprintf(seg_num_str, sizeof(seg_num_str), "%06d", state.current_segment);
    
    std::string segment_name = channel_name + "-" + seg_num_str;
    fs::path segment_path = state.path / (segment_name + ".segd");
    fs::create_directories(segment_path);
    
    // Create data file
    fs::path data_path = segment_path / (segment_name + ".tdat");
    
    // Close any existing data file for this channel
    auto file_it = m_impl->data_files.find(channel_name);
    if (file_it != m_impl->data_files.end()) {
        file_it->second.close();
        m_impl->data_files.erase(file_it);
    }
    
    // Open new data file
    std::ofstream data_file(data_path, std::ios::binary);
    if (!data_file) {
        throw std::runtime_error("Cannot create data file: " + data_path.string());
    }
    
    // Write universal header placeholder
    UniversalHeader uh;
    uh.set_file_type(TIME_SERIES_DATA_FILE_TYPE_STRING);
    uh.set_channel_name(channel_name);
    uh.set_session_name(m_session_name);
    uh.segment_number = state.current_segment;
    std::memcpy(uh.level_UUID.data(), m_impl->session_uuid.data(), UUID_BYTES);
    
    // Compute header CRC (excluding the CRC field itself)
    uh.header_CRC = CRC32::calculate(
        reinterpret_cast<const ui1*>(&uh) + sizeof(ui4),
        UNIVERSAL_HEADER_BYTES - sizeof(ui4));
    
    data_file.write(reinterpret_cast<const char*>(&uh), sizeof(uh));
    
    m_impl->data_files[channel_name] = std::move(data_file);
    m_impl->data_file_offsets[channel_name] = UNIVERSAL_HEADER_BYTES;
    
    // Reset segment-specific counters (but preserve total)
    state.indices.clear();
    state.last_sample_index = state.total_samples;
}

void MefWriter::write_data(const std::vector<sf8>& data,
                            const std::string& channel_name,
                            si8 start_uutc,
                            sf8 sampling_freq,
                            si4 precision,
                            bool new_segment) {
    write_data(data.data(), data.size(), channel_name, start_uutc, 
               sampling_freq, precision, new_segment);
}

void MefWriter::write_data(const sf8* data,
                            size_t num_samples,
                            const std::string& channel_name,
                            si8 start_uutc,
                            sf8 sampling_freq,
                            si4 precision,
                            bool new_segment) {
    if (m_closed) {
        throw std::runtime_error("Writer is closed");
    }
    
    if (num_samples == 0) {
        return;
    }
    
    // Calculate conversion factor based on precision
    sf8 scale_factor = 1.0;
    if (precision >= 0) {
        scale_factor = std::pow(10.0, precision);
    } else {
        // Auto-detect precision from data
        sf8 max_val = 0.0;
        for (size_t i = 0; i < num_samples; ++i) {
            if (!std::isnan(data[i])) {
                max_val = std::max(max_val, std::abs(data[i]));
            }
        }
        // Scale to use full si4 range (approximately)
        if (max_val > 0) {
            scale_factor = static_cast<sf8>(RED_MAXIMUM_SAMPLE_VALUE) / max_val * 0.9;
        }
    }
    
    // Convert to si4
    std::vector<si4> samples(num_samples);
    for (size_t i = 0; i < num_samples; ++i) {
        if (std::isnan(data[i])) {
            samples[i] = RED_NAN;
        } else {
            sf8 scaled = data[i] * scale_factor;
            // Clamp to valid range
            scaled = std::clamp(scaled, 
                               static_cast<sf8>(RED_MINIMUM_SAMPLE_VALUE),
                               static_cast<sf8>(RED_MAXIMUM_SAMPLE_VALUE));
            samples[i] = static_cast<si4>(std::round(scaled));
        }
    }
    
    // Store conversion factor in units
    if (scale_factor != 1.0) {
        m_units_conversion_factor = 1.0 / scale_factor;
    }
    
    write_raw_data(samples, channel_name, start_uutc, sampling_freq, new_segment);
}

void MefWriter::write_raw_data(const std::vector<si4>& data,
                                const std::string& channel_name,
                                si8 start_uutc,
                                sf8 sampling_freq,
                                bool new_segment) {
    if (m_closed) {
        throw std::runtime_error("Writer is closed");
    }
    
    if (data.empty()) {
        return;
    }
    
    // Ensure channel exists
    ensure_channel(channel_name, sampling_freq);
    auto& state = m_channel_states[channel_name];
    
    // Check if we need a new segment
    bool need_new_segment = new_segment || (state.current_segment < 0);
    
    // Check for time discontinuity (more than 2 blocks worth of gap)
    if (!need_new_segment && state.last_end_time != UUTC_NO_ENTRY) {
        si8 expected_start = state.last_end_time + static_cast<si8>(1e6 / sampling_freq);
        si8 gap = start_uutc - expected_start;
        si8 max_gap = static_cast<si8>(2.0 * m_block_len * 1e6 / sampling_freq);
        
        if (std::abs(gap) > max_gap) {
            need_new_segment = true;
        }
    }
    
    if (need_new_segment) {
        // Finalize current segment if exists
        if (state.current_segment >= 0) {
            finalize_segment(channel_name, state.current_segment);
        }
        create_segment(channel_name);
    }
    
    // Write data in blocks
    size_t samples_written = 0;
    bool first_block_in_write = true;
    
    while (samples_written < data.size()) {
        // Determine block size
        size_t remaining = data.size() - samples_written;
        ui4 block_samples = static_cast<ui4>(std::min(remaining, 
                                                       static_cast<size_t>(m_block_len)));
        
        // Calculate block start time
        si8 block_time = start_uutc + static_cast<si8>(samples_written * 1e6 / sampling_freq);
        
        // Write block
        bool is_discontinuity = first_block_in_write && need_new_segment;
        write_block(channel_name, data.data() + samples_written, block_samples, 
                    block_time, is_discontinuity);
        
        samples_written += block_samples;
        first_block_in_write = false;
    }
    
    // Update state
    state.last_end_time = start_uutc + static_cast<si8>((data.size() - 1) * 1e6 / sampling_freq);
    state.total_samples += static_cast<si8>(data.size());
}

void MefWriter::write_block(const std::string& channel_name,
                             const si4* samples, ui4 num_samples, si8 start_time,
                             bool is_discontinuity) {
    auto& state = m_channel_states[channel_name];
    
    // Compress the block
    REDCodec::CompressionParams params;
    params.discontinuity = is_discontinuity;
    
    auto result = REDCodec::compress(samples, num_samples, start_time, params);
    if (!result.success) {
        throw std::runtime_error("Compression failed");
    }
    
    // Get current file offset
    si8 file_offset = m_impl->data_file_offsets[channel_name];
    
    // Update index
    result.index.file_offset = file_offset;
    result.index.start_sample = state.last_sample_index;
    state.indices.push_back(result.index);
    
    // Write compressed data
    auto& data_file = m_impl->data_files[channel_name];
    data_file.write(reinterpret_cast<const char*>(result.compressed_data.data()),
                    static_cast<std::streamsize>(result.compressed_data.size()));
    
    // Update offset
    m_impl->data_file_offsets[channel_name] += static_cast<si8>(result.compressed_data.size());
    
    // Update sample index
    state.last_sample_index += num_samples;
    state.total_blocks++;
}

void MefWriter::finalize_segment(const std::string& channel_name, si4 segment_num) {
    // Close data file
    auto file_it = m_impl->data_files.find(channel_name);
    if (file_it != m_impl->data_files.end()) {
        file_it->second.close();
        m_impl->data_files.erase(file_it);
    }
    
    // Write metadata and indices
    write_metadata(channel_name, segment_num);
    write_indices(channel_name, segment_num);
}

void MefWriter::write_metadata(const std::string& channel_name, si4 segment_num) {
    auto& state = m_channel_states[channel_name];
    
    // Format segment name
    char seg_num_str[16];
    snprintf(seg_num_str, sizeof(seg_num_str), "%06d", segment_num);
    std::string segment_name = channel_name + "-" + seg_num_str;
    
    fs::path segment_path = state.path / (segment_name + ".segd");
    fs::path meta_path = segment_path / (segment_name + ".tmet");
    
    std::ofstream file(meta_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot create metadata file: " + meta_path.string());
    }
    
    // Calculate time bounds from indices
    si8 start_time = UUTC_NO_ENTRY;
    si8 end_time = UUTC_NO_ENTRY;
    si8 total_samples = 0;
    
    if (!state.indices.empty()) {
        start_time = state.indices.front().start_time;
        const auto& last = state.indices.back();
        end_time = last.start_time + static_cast<si8>(
            (last.number_of_samples - 1) * 1e6 / state.sampling_frequency);
        
        for (const auto& idx : state.indices) {
            total_samples += idx.number_of_samples;
        }
    }
    
    // Create and write universal header
    UniversalHeader uh;
    uh.set_file_type(TIME_SERIES_METADATA_FILE_TYPE_STRING);
    uh.set_channel_name(channel_name);
    uh.set_session_name(m_session_name);
    uh.segment_number = segment_num;
    uh.start_time = start_time;
    uh.end_time = end_time;
    uh.number_of_entries = 1;
    std::memcpy(uh.level_UUID.data(), m_impl->session_uuid.data(), UUID_BYTES);
    
    file.write(reinterpret_cast<const char*>(&uh), sizeof(uh));
    
    // Write metadata section 1
    MetadataSection1 meta1;
    file.seekp(UNIVERSAL_HEADER_BYTES);
    
    // Pad to section 2 offset
    std::vector<ui1> padding(METADATA_SECTION_2_OFFSET - file.tellp(), PAD_BYTE_VALUE);
    file.write(reinterpret_cast<const char*>(padding.data()), 
               static_cast<std::streamsize>(padding.size()));
    
    // Write time series metadata section 2
    TimeSeriesMetadataSection2 meta2;
    meta2.sampling_frequency = state.sampling_frequency;
    meta2.number_of_samples = total_samples;
    meta2.number_of_blocks = static_cast<si8>(state.indices.size());
    meta2.units_conversion_factor = m_units_conversion_factor;
    meta2.set_units_description(m_data_units);
    
    // Copy description strings
    std::strncpy(meta2.channel_description.data(), m_channel_description.c_str(),
                 METADATA_CHANNEL_DESCRIPTION_BYTES - 1);
    std::strncpy(meta2.session_description.data(), m_session_description.c_str(),
                 METADATA_SESSION_DESCRIPTION_BYTES - 1);
    
    if (start_time != UUTC_NO_ENTRY && end_time != UUTC_NO_ENTRY) {
        meta2.recording_duration = end_time - start_time;
    }
    
    // Calculate block statistics
    if (!state.indices.empty()) {
        ui4 max_samples = 0;
        si8 max_bytes = 0;
        
        for (const auto& idx : state.indices) {
            max_samples = std::max(max_samples, idx.number_of_samples);
            max_bytes = std::max(max_bytes, static_cast<si8>(idx.block_bytes));
        }
        
        meta2.maximum_block_samples = max_samples;
        meta2.maximum_block_bytes = max_bytes;
        
        if (state.sampling_frequency > 0) {
            meta2.block_interval = static_cast<si8>(max_samples * 1e6 / state.sampling_frequency);
        }
    }
    
    file.write(reinterpret_cast<const char*>(&meta2), sizeof(meta2));
    
    // Write metadata section 3
    file.seekp(METADATA_SECTION_3_OFFSET);
    MetadataSection3 meta3;
    meta3.recording_time_offset = m_recording_time_offset;
    meta3.GMT_offset = m_gmt_offset;
    
    std::strncpy(meta3.subject_name_1.data(), m_subject_name.c_str(),
                 METADATA_SUBJECT_NAME_BYTES - 1);
    std::strncpy(meta3.subject_ID.data(), m_subject_id.c_str(),
                 METADATA_SUBJECT_ID_BYTES - 1);
    std::strncpy(meta3.recording_location.data(), m_recording_location.c_str(),
                 METADATA_RECORDING_LOCATION_BYTES - 1);
    
    file.write(reinterpret_cast<const char*>(&meta3), sizeof(meta3));
    
    // Pad to full metadata file size
    file.seekp(0, std::ios::end);
    si8 current_size = file.tellp();
    if (current_size < METADATA_FILE_BYTES) {
        padding.resize(METADATA_FILE_BYTES - current_size, PAD_BYTE_VALUE);
        file.write(reinterpret_cast<const char*>(padding.data()),
                   static_cast<std::streamsize>(padding.size()));
    }
}

void MefWriter::write_indices(const std::string& channel_name, si4 segment_num) {
    auto& state = m_channel_states[channel_name];
    
    // Format segment name
    char seg_num_str[16];
    snprintf(seg_num_str, sizeof(seg_num_str), "%06d", segment_num);
    std::string segment_name = channel_name + "-" + seg_num_str;
    
    fs::path segment_path = state.path / (segment_name + ".segd");
    fs::path idx_path = segment_path / (segment_name + ".tidx");
    
    std::ofstream file(idx_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot create index file: " + idx_path.string());
    }
    
    // Calculate time bounds
    si8 start_time = UUTC_NO_ENTRY;
    si8 end_time = UUTC_NO_ENTRY;
    si8 max_entry_size = 0;
    
    if (!state.indices.empty()) {
        start_time = state.indices.front().start_time;
        const auto& last = state.indices.back();
        end_time = last.start_time + static_cast<si8>(
            (last.number_of_samples - 1) * 1e6 / state.sampling_frequency);
        
        for (const auto& idx : state.indices) {
            max_entry_size = std::max(max_entry_size, static_cast<si8>(idx.block_bytes));
        }
    }
    
    // Create universal header
    UniversalHeader uh;
    uh.set_file_type(TIME_SERIES_INDICES_FILE_TYPE_STRING);
    uh.set_channel_name(channel_name);
    uh.set_session_name(m_session_name);
    uh.segment_number = segment_num;
    uh.start_time = start_time;
    uh.end_time = end_time;
    uh.number_of_entries = static_cast<si8>(state.indices.size());
    uh.maximum_entry_size = max_entry_size;
    std::memcpy(uh.level_UUID.data(), m_impl->session_uuid.data(), UUID_BYTES);
    
    // Calculate CRCs
    uh.body_CRC = CRC32::calculate(
        reinterpret_cast<const ui1*>(state.indices.data()),
        state.indices.size() * sizeof(TimeSeriesIndex));
    
    file.write(reinterpret_cast<const char*>(&uh), sizeof(uh));
    
    // Write indices
    file.write(reinterpret_cast<const char*>(state.indices.data()),
               static_cast<std::streamsize>(state.indices.size() * sizeof(TimeSeriesIndex)));
}

void MefWriter::flush() {
    for (auto& [channel_name, file] : m_impl->data_files) {
        if (file.is_open()) {
            file.flush();
        }
    }
}

void MefWriter::close() {
    if (m_closed) {
        return;
    }
    
    // Finalize all channels
    for (auto& [channel_name, state] : m_channel_states) {
        if (state.current_segment >= 0) {
            finalize_segment(channel_name, state.current_segment);
        }
    }
    
    m_closed = true;
}

} // namespace brainmaze_mefd
