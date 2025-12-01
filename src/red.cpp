/**
 * @file red.cpp
 * @brief RED (Range Encoded Differences) codec implementation
 */

#include "brainmaze_mefd/red.hpp"
#include "brainmaze_mefd/crc.hpp"
#include "brainmaze_mefd/aes.hpp"
#include <algorithm>
#include <cstring>
#include <cmath>
#include <limits>

namespace brainmaze_mefd {

// Normal CDF table for normality testing
const std::array<sf8, RED_NORMAL_CDF_TABLE_ENTRIES> REDCodec::s_normal_cdf_table = {{
    0.00134989803163010, 0.00186581330038404, 0.00255513033042794, 0.00346697380304067,
    0.00466118802371875, 0.00620966532577614, 0.00819753592459614, 0.01072411002167580,
    0.01390344751349860, 0.01786442056281660, 0.02275013194817920, 0.02871655981600180,
    0.03593031911292580, 0.04456546275854310, 0.05479929169955800, 0.06680720126885810,
    0.08075665923377110, 0.09680048458561040, 0.11506967022170800, 0.13566606094638300,
    0.15865525393145700, 0.18406012534676000, 0.21185539858339700, 0.24196365222307300,
    0.27425311775007400, 0.30853753872598700, 0.34457825838967600, 0.38208857781104700,
    0.42074029056089700, 0.46017216272297100, 0.50000000000000000, 0.53982783727702900,
    0.57925970943910300, 0.61791142218895300, 0.65542174161032400, 0.69146246127401300,
    0.72574688224992600, 0.75803634777692700, 0.78814460141660300, 0.81593987465324100,
    0.84134474606854300, 0.86433393905361700, 0.88493032977829200, 0.90319951541439000,
    0.91924334076622900, 0.93319279873114200, 0.94520070830044200, 0.95543453724145700,
    0.96406968088707400, 0.97128344018399800, 0.97724986805182100, 0.98213557943718300,
    0.98609655248650100, 0.98927588997832400, 0.99180246407540400, 0.99379033467422400,
    0.99533881197628100, 0.99653302619695900, 0.99744486966957200, 0.99813418669961600,
    0.99865010196837000
}};

void REDCodec::find_extrema(const si4* samples, ui4 num_samples, si4& min_val, si4& max_val) {
    if (num_samples == 0) {
        min_val = RED_NAN;
        max_val = RED_NAN;
        return;
    }
    
    min_val = RED_MAXIMUM_SAMPLE_VALUE;
    max_val = RED_MINIMUM_SAMPLE_VALUE;
    
    for (ui4 i = 0; i < num_samples; ++i) {
        si4 val = samples[i];
        if (val == RED_NAN) continue;  // Skip NaN values
        
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }
}

void REDCodec::compute_statistics(const si4* differences, ui4 num_samples,
                                   std::array<ui1, RED_BLOCK_STATISTICS_BYTES>& stats) {
    // Initialize counts to zero
    std::array<ui4, 256> counts{};
    
    // Count symbol frequencies
    for (ui4 i = 0; i < num_samples; ++i) {
        si4 diff = differences[i];
        // Map difference to a byte value (simplified)
        ui1 symbol = static_cast<ui1>((diff + 128) & 0xFF);
        counts[symbol]++;
    }
    
    // Normalize counts to fit in ui1 (max 255)
    ui4 max_count = *std::max_element(counts.begin(), counts.end());
    if (max_count > 0) {
        for (int i = 0; i < 256; ++i) {
            stats[i] = static_cast<ui1>((counts[i] * 255) / max_count);
            if (counts[i] > 0 && stats[i] == 0) stats[i] = 1;  // Ensure non-zero counts stay non-zero
        }
    }
}

void REDCodec::encode_differences(const si4* samples, ui4 num_samples,
                                   std::vector<ui1>& output,
                                   std::array<ui4, 257>& counts) {
    // Calculate differences
    std::vector<si4> differences(num_samples);
    differences[0] = samples[0];  // First sample stored directly
    
    for (ui4 i = 1; i < num_samples; ++i) {
        differences[i] = samples[i] - samples[i - 1];
    }
    
    // Initialize counts
    counts.fill(0);
    
    // Simple variable-length encoding for differences
    // Encoding scheme:
    //   0xxxxxxx         = positive 0-127
    //   10xxxxxx         = negative -(x+1) where x = 0-63, so -1 to -64
    //   110xxxxx xxxxxxxx = 13-bit signed value (-4096 to 4095)
    //   1110xxxx xxxxxxxx xxxxxxxx = 20-bit signed value
    //   11110000 xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx = full 32-bit
    
    for (ui4 i = 0; i < num_samples; ++i) {
        si4 diff = differences[i];
        
        if (diff >= 0 && diff <= 127) {
            // 1-byte positive: 0xxxxxxx
            output.push_back(static_cast<ui1>(diff));
            counts[0]++;
        } else if (diff >= -64 && diff < 0) {
            // 1-byte negative: 10xxxxxx (stores -diff-1)
            output.push_back(static_cast<ui1>(0x80 | (-diff - 1)));
            counts[0]++;
        } else if (diff >= -4096 && diff <= 4095) {
            // 2-byte: 110sxxxx xxxxxxxx (13-bit signed, s=sign)
            si4 val = diff >= 0 ? diff : (-diff - 1);
            output.push_back(static_cast<ui1>(0xC0 | ((val >> 8) & 0x0F) | (diff < 0 ? 0x10 : 0)));
            output.push_back(static_cast<ui1>(val & 0xFF));
            counts[1]++;
        } else if (diff >= -524288 && diff <= 524287) {
            // 3-byte: 1110sxxx xxxxxxxx xxxxxxxx (20-bit signed, s=sign)
            si4 val = diff >= 0 ? diff : (-diff - 1);
            output.push_back(static_cast<ui1>(0xE0 | ((val >> 16) & 0x07) | (diff < 0 ? 0x08 : 0)));
            output.push_back(static_cast<ui1>((val >> 8) & 0xFF));
            output.push_back(static_cast<ui1>(val & 0xFF));
            counts[2]++;
        } else {
            // 5-byte: 11110000 xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx (full 32-bit)
            output.push_back(0xF0);
            // Store as-is (two's complement)
            output.push_back(static_cast<ui1>((diff >> 24) & 0xFF));
            output.push_back(static_cast<ui1>((diff >> 16) & 0xFF));
            output.push_back(static_cast<ui1>((diff >> 8) & 0xFF));
            output.push_back(static_cast<ui1>(diff & 0xFF));
            counts[4]++;
        }
    }
}

void REDCodec::decode_differences(const ui1* input, ui4 diff_bytes,
                                   const std::array<ui1, RED_BLOCK_STATISTICS_BYTES>& /* statistics */,
                                   si4* output, ui4 num_samples) {
    ui4 pos = 0;
    si4 prev = 0;
    
    for (ui4 i = 0; i < num_samples && pos < diff_bytes; ++i) {
        si4 diff;
        ui1 byte = input[pos++];
        
        if ((byte & 0x80) == 0) {
            // 1-byte positive: 0xxxxxxx
            diff = byte;
        } else if ((byte & 0xC0) == 0x80) {
            // 1-byte negative: 10xxxxxx
            diff = -static_cast<si4>(byte & 0x3F) - 1;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2-byte: 110sxxxx xxxxxxxx
            bool negative = (byte & 0x10) != 0;
            si4 val = ((byte & 0x0F) << 8) | input[pos++];
            diff = negative ? (-val - 1) : val;
        } else if ((byte & 0xF0) == 0xE0) {
            // 3-byte: 1110sxxx xxxxxxxx xxxxxxxx
            bool negative = (byte & 0x08) != 0;
            si4 val = ((byte & 0x07) << 16) | (input[pos] << 8) | input[pos + 1];
            pos += 2;
            diff = negative ? (-val - 1) : val;
        } else {
            // 5-byte: 11110000 + 4 bytes (full 32-bit two's complement)
            diff = static_cast<si4>(
                (static_cast<ui4>(input[pos]) << 24) | 
                (static_cast<ui4>(input[pos + 1]) << 16) | 
                (static_cast<ui4>(input[pos + 2]) << 8) | 
                static_cast<ui4>(input[pos + 3]));
            pos += 4;
        }
        
        if (i == 0) {
            output[i] = diff;  // First sample is stored directly
            prev = diff;
        } else {
            output[i] = prev + diff;
            prev = output[i];
        }
    }
}

REDCodec::CompressionResult REDCodec::compress(const si4* samples, ui4 num_samples,
                                                si8 start_time, const CompressionParams& params) {
    CompressionResult result;
    
    if (num_samples == 0 || samples == nullptr) {
        return result;
    }
    
    // Reserve space for compressed output
    result.compressed_data.reserve(RED_BLOCK_HEADER_BYTES + RED_MAX_DIFFERENCE_BYTES(num_samples));
    result.compressed_data.resize(RED_BLOCK_HEADER_BYTES);  // Make space for header
    
    // Encode differences
    std::array<ui4, 257> counts{};
    std::vector<ui1> diff_encoded;
    diff_encoded.reserve(RED_MAX_DIFFERENCE_BYTES(num_samples));
    encode_differences(samples, num_samples, diff_encoded, counts);
    
    // Append encoded differences to output
    result.compressed_data.insert(result.compressed_data.end(), 
                                   diff_encoded.begin(), diff_encoded.end());
    
    // Pad to 8-byte boundary
    while (result.compressed_data.size() % 8 != 0) {
        result.compressed_data.push_back(PAD_BYTE_VALUE);
    }
    
    // Find extrema
    si4 min_val, max_val;
    find_extrema(samples, num_samples, min_val, max_val);
    
    // Fill block header
    result.block_header.clear();
    result.block_header.flags = params.discontinuity ? RED_DISCONTINUITY_MASK : 0;
    result.block_header.scale_factor = 1.0f;
    result.block_header.difference_bytes = static_cast<ui4>(diff_encoded.size());
    result.block_header.number_of_samples = num_samples;
    result.block_header.block_bytes = static_cast<ui4>(result.compressed_data.size());
    result.block_header.start_time = start_time;
    
    // Compute and store statistics
    std::vector<si4> differences(num_samples);
    differences[0] = samples[0];
    for (ui4 i = 1; i < num_samples; ++i) {
        differences[i] = samples[i] - samples[i - 1];
    }
    compute_statistics(differences.data(), num_samples, result.block_header.statistics);
    
    // Copy header to output
    std::memcpy(result.compressed_data.data(), &result.block_header, RED_BLOCK_HEADER_BYTES);
    
    // Calculate CRC (excluding the CRC field itself)
    result.block_header.block_CRC = CRC32::calculate(
        result.compressed_data.data() + 4,
        result.compressed_data.size() - 4
    );
    
    // Update CRC in output
    std::memcpy(result.compressed_data.data(), &result.block_header.block_CRC, sizeof(ui4));
    
    // Fill index
    result.index.clear();
    result.index.file_offset = 0;  // Caller should set this
    result.index.start_time = start_time;
    result.index.start_sample = 0;  // Caller should set this
    result.index.number_of_samples = num_samples;
    result.index.block_bytes = static_cast<ui4>(result.compressed_data.size());
    result.index.maximum_sample_value = max_val;
    result.index.minimum_sample_value = min_val;
    result.index.RED_block_flags = result.block_header.flags;
    
    result.success = true;
    return result;
}

REDCodec::DecompressionResult REDCodec::decompress(const ui1* compressed_data,
                                                    size_t compressed_size,
                                                    const PasswordData* password_data) {
    DecompressionResult result;
    
    if (compressed_size < RED_BLOCK_HEADER_BYTES || compressed_data == nullptr) {
        return result;
    }
    
    // Parse block header
    std::memcpy(&result.block_header, compressed_data, RED_BLOCK_HEADER_BYTES);
    
    // Decompress using the header
    return decompress(result.block_header, 
                      compressed_data + RED_BLOCK_HEADER_BYTES,
                      password_data);
}

REDCodec::DecompressionResult REDCodec::decompress(const REDBlockHeader& block_header,
                                                    const ui1* compressed_data,
                                                    const PasswordData* password_data) {
    DecompressionResult result;
    result.block_header = block_header;
    
    if (block_header.number_of_samples == 0) {
        result.success = true;
        return result;
    }
    
    // TODO: Handle decryption if needed
    (void)password_data;  // Suppress unused warning
    
    // Allocate output buffer
    result.samples.resize(block_header.number_of_samples);
    
    // Decode differences
    decode_differences(compressed_data, block_header.difference_bytes,
                       block_header.statistics,
                       result.samples.data(), block_header.number_of_samples);
    
    // Apply scale factor if needed (for lossy compression)
    if (block_header.scale_factor != 1.0f && block_header.scale_factor != 0.0f) {
        for (auto& sample : result.samples) {
            sample = static_cast<si4>(std::round(sample * block_header.scale_factor));
        }
    }
    
    result.success = true;
    return result;
}

} // namespace brainmaze_mefd
