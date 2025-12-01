/**
 * @file mef_reader.cpp
 * @brief MEF 3.0 file reader implementation
 */

#include "brainmaze_mefd/mef_reader.hpp"
#include "brainmaze_mefd/red.hpp"
#include "brainmaze_mefd/crc.hpp"
#include "brainmaze_mefd/aes.hpp"
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace brainmaze_mefd {

namespace fs = std::filesystem;

// Internal implementation details
struct MefReader::Impl {
    PasswordData password_data;
    std::map<std::string, std::vector<SegmentInfo>> segment_info;
    std::map<std::string, std::vector<std::vector<TimeSeriesIndex>>> indices;
    std::map<std::string, TimeSeriesMetadataSection2> metadata_section2;
    std::map<std::string, MetadataSection3> metadata_section3;
};

MefReader::MefReader(const std::string& path, const std::string& password)
    : m_impl(std::make_unique<Impl>())
    , m_path(path)
    , m_password(password)
{
    m_valid = load_session();
}

MefReader::~MefReader() = default;

MefReader::MefReader(MefReader&& other) noexcept = default;
MefReader& MefReader::operator=(MefReader&& other) noexcept = default;

bool MefReader::load_session() {
    fs::path session_path(m_path);
    
    // Verify path exists and is a directory
    if (!fs::exists(session_path)) {
        return false;
    }
    
    if (!fs::is_directory(session_path)) {
        return false;
    }
    
    // Get session name from path
    m_session_name = session_path.stem().string();
    if (m_session_name.ends_with(".mefd")) {
        m_session_name = m_session_name.substr(0, m_session_name.length() - 5);
    }
    
    // Initialize time bounds
    m_start_time = std::numeric_limits<si8>::max();
    m_end_time = std::numeric_limits<si8>::min();
    
    // Scan for channel directories
    for (const auto& entry : fs::directory_iterator(session_path)) {
        if (entry.is_directory()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".timd") {  // Time series channel
                if (!load_channel(entry.path())) {
                    // Continue loading other channels
                }
            }
            // Note: Video channels (.vidd) are not supported per requirements
        }
    }
    
    // Update session times from channels
    for (const auto& [name, info] : m_channels) {
        if (info.start_time != UUTC_NO_ENTRY && info.start_time < m_start_time) {
            m_start_time = info.start_time;
        }
        if (info.end_time != UUTC_NO_ENTRY && info.end_time > m_end_time) {
            m_end_time = info.end_time;
        }
    }
    
    if (m_start_time == std::numeric_limits<si8>::max()) {
        m_start_time = UUTC_NO_ENTRY;
    }
    if (m_end_time == std::numeric_limits<si8>::min()) {
        m_end_time = UUTC_NO_ENTRY;
    }
    
    return !m_channels.empty();
}

bool MefReader::load_channel(const fs::path& channel_path) {
    std::string channel_name = channel_path.stem().string();
    
    ChannelInfo info;
    info.name = channel_name;
    info.channel_type = TIME_SERIES_CHANNEL_TYPE;
    info.start_time = std::numeric_limits<si8>::max();
    info.end_time = std::numeric_limits<si8>::min();
    
    // Initialize segment storage
    m_impl->segment_info[channel_name] = {};
    m_impl->indices[channel_name] = {};
    
    // Scan for segment directories
    std::vector<fs::path> segments;
    for (const auto& entry : fs::directory_iterator(channel_path)) {
        if (entry.is_directory() && entry.path().extension() == ".segd") {
            segments.push_back(entry.path());
        }
    }
    
    // Sort segments by number
    std::sort(segments.begin(), segments.end(), [](const fs::path& a, const fs::path& b) {
        return a.filename().string() < b.filename().string();
    });
    
    info.number_of_segments = static_cast<si4>(segments.size());
    
    // Load each segment
    for (const auto& seg_path : segments) {
        if (!load_segment(seg_path, channel_name)) {
            // Continue with other segments
        }
    }
    
    // Aggregate segment info
    for (const auto& seg : m_impl->segment_info[channel_name]) {
        info.number_of_samples += seg.number_of_samples;
        
        if (seg.start_time != UUTC_NO_ENTRY && seg.start_time < info.start_time) {
            info.start_time = seg.start_time;
        }
        if (seg.end_time != UUTC_NO_ENTRY && seg.end_time > info.end_time) {
            info.end_time = seg.end_time;
        }
    }
    
    // Copy metadata from first segment
    if (m_impl->metadata_section2.count(channel_name)) {
        const auto& meta2 = m_impl->metadata_section2[channel_name];
        info.sampling_frequency = meta2.sampling_frequency;
        info.units = std::string(meta2.units_description.data(), 
                                  std::strlen(meta2.units_description.data()));
        info.units_conversion_factor = meta2.units_conversion_factor;
    }
    
    if (info.start_time == std::numeric_limits<si8>::max()) {
        info.start_time = UUTC_NO_ENTRY;
    }
    if (info.end_time == std::numeric_limits<si8>::min()) {
        info.end_time = UUTC_NO_ENTRY;
    }
    
    m_channels[channel_name] = info;
    return true;
}

bool MefReader::load_segment(const fs::path& segment_path, const std::string& channel_name) {
    SegmentInfo seg_info;
    seg_info.name = segment_path.stem().string();
    
    // Parse segment number from name (format: channel_name-NNNNNN)
    size_t dash_pos = seg_info.name.rfind('-');
    if (dash_pos != std::string::npos) {
        try {
            seg_info.segment_number = std::stoi(seg_info.name.substr(dash_pos + 1));
        } catch (...) {
            seg_info.segment_number = 0;
        }
    }
    
    // Read metadata file
    fs::path meta_path = segment_path / (seg_info.name + ".tmet");
    if (fs::exists(meta_path)) {
        std::ifstream meta_file(meta_path, std::ios::binary);
        if (meta_file) {
            // Read universal header
            UniversalHeader uh;
            meta_file.read(reinterpret_cast<char*>(&uh), sizeof(uh));
            
            seg_info.start_time = uh.start_time;
            seg_info.end_time = uh.end_time;
            
            // Read metadata section 1
            MetadataSection1 meta1;
            meta_file.seekg(UNIVERSAL_HEADER_BYTES);
            // Section 1 is implicit in the structure
            
            // Read time series metadata section 2
            meta_file.seekg(METADATA_SECTION_2_OFFSET);
            TimeSeriesMetadataSection2 meta2;
            meta_file.read(reinterpret_cast<char*>(&meta2), sizeof(meta2));
            
            seg_info.number_of_samples = meta2.number_of_samples;
            seg_info.start_sample = meta2.start_sample;
            seg_info.number_of_blocks = meta2.number_of_blocks;
            
            // Store metadata for the channel (from first segment)
            if (!m_impl->metadata_section2.count(channel_name)) {
                m_impl->metadata_section2[channel_name] = meta2;
            }
            
            // Read metadata section 3
            meta_file.seekg(METADATA_SECTION_3_OFFSET);
            MetadataSection3 meta3;
            meta_file.read(reinterpret_cast<char*>(&meta3), sizeof(meta3));
            
            if (!m_impl->metadata_section3.count(channel_name)) {
                m_impl->metadata_section3[channel_name] = meta3;
            }
        }
    }
    
    // Read indices file
    fs::path idx_path = segment_path / (seg_info.name + ".tidx");
    if (fs::exists(idx_path)) {
        auto indices = read_indices(idx_path);
        m_impl->indices[channel_name].push_back(indices);
    }
    
    m_impl->segment_info[channel_name].push_back(seg_info);
    return true;
}

std::vector<TimeSeriesIndex> MefReader::read_indices(const fs::path& indices_path) const {
    std::vector<TimeSeriesIndex> indices;
    
    std::ifstream file(indices_path, std::ios::binary);
    if (!file) {
        return indices;
    }
    
    // Read universal header
    UniversalHeader uh;
    file.read(reinterpret_cast<char*>(&uh), sizeof(uh));
    
    si8 num_entries = uh.number_of_entries;
    if (num_entries <= 0) {
        return indices;
    }
    
    // Read index entries
    indices.resize(static_cast<size_t>(num_entries));
    file.read(reinterpret_cast<char*>(indices.data()), 
              num_entries * sizeof(TimeSeriesIndex));
    
    return indices;
}

std::vector<std::string> MefReader::get_channels() const {
    std::vector<std::string> names;
    names.reserve(m_channels.size());
    for (const auto& [name, info] : m_channels) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> MefReader::get_time_series_channels() const {
    std::vector<std::string> names;
    for (const auto& [name, info] : m_channels) {
        if (info.channel_type == TIME_SERIES_CHANNEL_TYPE) {
            names.push_back(name);
        }
    }
    return names;
}

MefReader::ChannelInfo MefReader::get_channel_info(const std::string& channel_name) const {
    auto it = m_channels.find(channel_name);
    if (it == m_channels.end()) {
        throw std::runtime_error("Channel not found: " + channel_name);
    }
    return it->second;
}

std::vector<MefReader::SegmentInfo> MefReader::get_segments(const std::string& channel_name) const {
    auto it = m_impl->segment_info.find(channel_name);
    if (it == m_impl->segment_info.end()) {
        throw std::runtime_error("Channel not found: " + channel_name);
    }
    return it->second;
}

sf8 MefReader::get_numeric_property(const std::string& property_name,
                                     const std::string& channel_name) const {
    if (channel_name.empty()) {
        // Session-level properties
        if (property_name == "start_time") return static_cast<sf8>(m_start_time);
        if (property_name == "end_time") return static_cast<sf8>(m_end_time);
        if (property_name == "duration") return static_cast<sf8>(get_duration());
    } else {
        // Channel-level properties
        auto it = m_channels.find(channel_name);
        if (it == m_channels.end()) {
            throw std::runtime_error("Channel not found: " + channel_name);
        }
        
        const auto& info = it->second;
        if (property_name == "fsamp" || property_name == "sampling_frequency") {
            return info.sampling_frequency;
        }
        if (property_name == "num_samples" || property_name == "number_of_samples") {
            return static_cast<sf8>(info.number_of_samples);
        }
        if (property_name == "start_time") {
            return static_cast<sf8>(info.start_time);
        }
        if (property_name == "end_time") {
            return static_cast<sf8>(info.end_time);
        }
        if (property_name == "units_conversion_factor") {
            return info.units_conversion_factor;
        }
    }
    
    throw std::runtime_error("Unknown property: " + property_name);
}

std::string MefReader::get_string_property(const std::string& property_name,
                                            const std::string& channel_name) const {
    if (channel_name.empty()) {
        // Session-level properties
        if (property_name == "session_name") return m_session_name;
        if (property_name == "path") return m_path;
    } else {
        // Channel-level properties
        auto it = m_channels.find(channel_name);
        if (it == m_channels.end()) {
            throw std::runtime_error("Channel not found: " + channel_name);
        }
        
        if (property_name == "unit" || property_name == "units") {
            return it->second.units;
        }
        if (property_name == "channel_name") {
            return it->second.name;
        }
    }
    
    throw std::runtime_error("Unknown property: " + property_name);
}

std::vector<sf8> MefReader::get_data(const std::string& channel_name,
                                      const si8* start_time,
                                      const si8* end_time) const {
    auto it = m_channels.find(channel_name);
    if (it == m_channels.end()) {
        throw std::runtime_error("Channel not found: " + channel_name);
    }
    
    const auto& info = it->second;
    sf8 fs = info.sampling_frequency;
    if (fs <= 0) {
        throw std::runtime_error("Invalid sampling frequency for channel: " + channel_name);
    }
    
    // Calculate sample range from time range
    si8 t_start = start_time ? *start_time : info.start_time;
    si8 t_end = end_time ? *end_time : info.end_time;
    
    si8 start_sample = static_cast<si8>((t_start - info.start_time) * fs / 1e6);
    si8 end_sample = static_cast<si8>((t_end - info.start_time) * fs / 1e6);
    
    // Clamp to valid range
    start_sample = std::max<si8>(0, start_sample);
    end_sample = std::min(end_sample, info.number_of_samples);
    
    // Get raw data
    auto raw = get_raw_data(channel_name, start_sample, end_sample);
    
    // Convert to float and apply units conversion
    std::vector<sf8> result(raw.size());
    sf8 conversion = info.units_conversion_factor;
    if (conversion == 0.0) conversion = 1.0;
    
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == RED_NAN) {
            result[i] = std::numeric_limits<sf8>::quiet_NaN();
        } else {
            result[i] = static_cast<sf8>(raw[i]) * conversion;
        }
    }
    
    return result;
}

std::vector<si4> MefReader::get_raw_data(const std::string& channel_name,
                                          si8 start_sample,
                                          si8 end_sample) const {
    auto ch_it = m_channels.find(channel_name);
    if (ch_it == m_channels.end()) {
        throw std::runtime_error("Channel not found: " + channel_name);
    }
    
    auto seg_it = m_impl->segment_info.find(channel_name);
    auto idx_it = m_impl->indices.find(channel_name);
    if (seg_it == m_impl->segment_info.end() || idx_it == m_impl->indices.end()) {
        throw std::runtime_error("No segment data for channel: " + channel_name);
    }
    
    std::vector<si4> result;
    result.reserve(static_cast<size_t>(end_sample - start_sample));
    
    // Find which segments contain the requested samples
    si8 accumulated_samples = 0;
    
    for (size_t seg_idx = 0; seg_idx < seg_it->second.size(); ++seg_idx) {
        const auto& seg = seg_it->second[seg_idx];
        si8 seg_start = accumulated_samples;
        si8 seg_end = accumulated_samples + seg.number_of_samples;
        
        // Check if this segment overlaps the requested range
        if (seg_end > start_sample && seg_start < end_sample) {
            // Get the indices for this segment
            const auto& indices = idx_it->second[seg_idx];
            
            // Build path to data file
            fs::path channel_path = fs::path(m_path) / (channel_name + ".timd");
            fs::path seg_path = channel_path / (seg.name + ".segd");
            fs::path data_path = seg_path / (seg.name + ".tdat");
            
            // Find relevant blocks
            si8 local_start = std::max<si8>(0, start_sample - seg_start);
            si8 local_end = std::min(seg.number_of_samples, end_sample - seg_start);
            
            // Decompress the required blocks
            auto samples = decompress_blocks(data_path, indices, local_start, local_end);
            
            // Append to result
            result.insert(result.end(), samples.begin(), samples.end());
        }
        
        accumulated_samples = seg_end;
    }
    
    return result;
}

std::vector<si4> MefReader::decompress_blocks(const fs::path& data_path,
                                               const std::vector<TimeSeriesIndex>& indices,
                                               si8 start_idx, si8 end_idx) const {
    std::vector<si4> result;
    
    if (indices.empty()) {
        return result;
    }
    
    std::ifstream file(data_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open data file: " + data_path.string());
    }
    
    // Skip universal header
    file.seekg(UNIVERSAL_HEADER_BYTES);
    
    // Find which blocks contain the requested samples
    for (size_t blk_idx = 0; blk_idx < indices.size(); ++blk_idx) {
        const auto& idx = indices[blk_idx];
        si8 blk_start = idx.start_sample;
        si8 blk_end = blk_start + idx.number_of_samples;
        
        // Check if this block overlaps the requested range
        if (blk_end > start_idx && blk_start < end_idx) {
            // Read and decompress the block
            std::vector<ui1> compressed(idx.block_bytes);
            file.seekg(idx.file_offset);
            file.read(reinterpret_cast<char*>(compressed.data()), idx.block_bytes);
            
            auto decomp_result = REDCodec::decompress(compressed.data(), compressed.size(),
                                                       &m_impl->password_data);
            
            if (decomp_result.success) {
                // Extract only the samples we need from this block
                si8 local_start = std::max<si8>(0, start_idx - blk_start);
                si8 local_end = std::min<si8>(idx.number_of_samples, end_idx - blk_start);
                
                for (si8 i = local_start; i < local_end; ++i) {
                    result.push_back(decomp_result.samples[i]);
                }
            }
        }
    }
    
    return result;
}

} // namespace brainmaze_mefd
