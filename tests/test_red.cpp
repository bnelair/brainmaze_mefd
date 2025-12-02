/**
 * @file test_red.cpp
 * @brief RED codec tests
 */

#include <brainmaze_mefd/red.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>

using namespace brainmaze_mefd;

bool test_red() {
    bool all_passed = true;
    
    // Test 1: Simple compress/decompress round-trip
    {
        std::vector<si4> samples = {100, 102, 105, 108, 110, 112, 115, 118, 120, 125};
        si8 start_time = 1000000;
        
        auto comp_result = REDCodec::compress(samples.data(), 
                                               static_cast<ui4>(samples.size()), 
                                               start_time);
        
        if (!comp_result.success) {
            std::cout << "  ERROR: Compression failed" << std::endl;
            all_passed = false;
        } else {
            std::cout << "  Compressed " << samples.size() << " samples to " 
                      << comp_result.compressed_data.size() << " bytes" << std::endl;
            
            auto decomp_result = REDCodec::decompress(comp_result.compressed_data.data(),
                                                       comp_result.compressed_data.size());
            
            if (!decomp_result.success) {
                std::cout << "  ERROR: Decompression failed" << std::endl;
                all_passed = false;
            } else if (decomp_result.samples.size() != samples.size()) {
                std::cout << "  ERROR: Sample count mismatch" << std::endl;
                all_passed = false;
            } else {
                bool match = true;
                for (size_t i = 0; i < samples.size(); ++i) {
                    if (decomp_result.samples[i] != samples[i]) {
                        std::cout << "  ERROR: Sample " << i << " mismatch: "
                                  << decomp_result.samples[i] << " != " << samples[i] << std::endl;
                        match = false;
                        break;
                    }
                }
                if (match) {
                    std::cout << "  Simple round-trip: OK" << std::endl;
                } else {
                    all_passed = false;
                }
            }
        }
    }
    
    // Test 2: Sine wave compression
    {
        const int NUM_SAMPLES = 1000;
        std::vector<si4> samples(NUM_SAMPLES);
        
        // Generate sine wave
        for (int i = 0; i < NUM_SAMPLES; ++i) {
            samples[i] = static_cast<si4>(1000 * std::sin(2 * M_PI * i / 100));
        }
        
        auto comp_result = REDCodec::compress(samples.data(), NUM_SAMPLES, 0);
        
        if (comp_result.success) {
            double compression_ratio = static_cast<double>(comp_result.compressed_data.size()) / 
                                       (NUM_SAMPLES * sizeof(si4));
            std::cout << "  Sine wave compression ratio: " << compression_ratio << std::endl;
            
            auto decomp_result = REDCodec::decompress(comp_result.compressed_data.data(),
                                                       comp_result.compressed_data.size());
            
            if (decomp_result.success && decomp_result.samples.size() == samples.size()) {
                bool match = true;
                for (size_t i = 0; i < samples.size(); ++i) {
                    if (decomp_result.samples[i] != samples[i]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    std::cout << "  Sine wave round-trip: OK" << std::endl;
                } else {
                    std::cout << "  ERROR: Sine wave round-trip failed" << std::endl;
                    all_passed = false;
                }
            } else {
                std::cout << "  ERROR: Sine wave decompression failed" << std::endl;
                all_passed = false;
            }
        } else {
            std::cout << "  ERROR: Sine wave compression failed" << std::endl;
            all_passed = false;
        }
    }
    
    // Test 3: Find extrema
    {
        std::vector<si4> samples = {-100, 50, 200, -300, 150, 0, 75};
        si4 min_val, max_val;
        
        REDCodec::find_extrema(samples.data(), static_cast<ui4>(samples.size()), min_val, max_val);
        
        if (min_val != -300 || max_val != 200) {
            std::cout << "  ERROR: find_extrema failed" << std::endl;
            all_passed = false;
        } else {
            std::cout << "  find_extrema: OK (min=" << min_val << ", max=" << max_val << ")" << std::endl;
        }
    }
    
    // Test 4: Random data compression
    {
        const int NUM_SAMPLES = 500;
        std::vector<si4> samples(NUM_SAMPLES);
        
        std::mt19937 gen(42);  // Fixed seed for reproducibility
        std::uniform_int_distribution<si4> dis(-1000, 1000);
        
        for (int i = 0; i < NUM_SAMPLES; ++i) {
            samples[i] = dis(gen);
        }
        
        auto comp_result = REDCodec::compress(samples.data(), NUM_SAMPLES, 0);
        
        if (comp_result.success) {
            auto decomp_result = REDCodec::decompress(comp_result.compressed_data.data(),
                                                       comp_result.compressed_data.size());
            
            if (decomp_result.success && decomp_result.samples.size() == samples.size()) {
                bool match = true;
                for (size_t i = 0; i < samples.size(); ++i) {
                    if (decomp_result.samples[i] != samples[i]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    std::cout << "  Random data round-trip: OK" << std::endl;
                } else {
                    std::cout << "  ERROR: Random data round-trip failed" << std::endl;
                    all_passed = false;
                }
            }
        }
    }
    
    return all_passed;
}
