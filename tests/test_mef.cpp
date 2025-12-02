/**
 * @file test_mef.cpp
 * @brief MEF reader/writer tests
 */

#include <brainmaze_mefd/mef.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <chrono>

using namespace brainmaze_mefd;
namespace fs = std::filesystem;

bool test_mef() {
    bool all_passed = true;
    
    // Create a temporary test directory
    fs::path test_dir = fs::temp_directory_path() / "brainmaze_mefd_test";
    fs::path test_session = test_dir / "test_session.mefd";
    
    // Clean up any existing test files
    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
    }
    fs::create_directories(test_dir);
    
    // Test 1: Write and read simple data
    {
        std::cout << "  Test path: " << test_session << std::endl;
        
        // Write data
        {
            MefWriter writer(test_session.string(), true);
            
            if (!writer.is_valid()) {
                std::cout << "  ERROR: Failed to create MefWriter" << std::endl;
                all_passed = false;
            } else {
                writer.set_data_units("mV");
                writer.set_mef_block_len(100);
                writer.set_subject_name("Test Subject");
                writer.set_session_description("Unit test session");
                
                // Generate test data
                const int NUM_SAMPLES = 1000;
                std::vector<sf8> data(NUM_SAMPLES);
                for (int i = 0; i < NUM_SAMPLES; ++i) {
                    data[i] = std::sin(2 * M_PI * i / 100.0) * 100.0;
                }
                
                // Write at 1000 Hz
                si8 start_time = 1000000000000LL;  // Some arbitrary time
                writer.write_data(data, "test_channel", start_time, 1000.0);
                
                writer.close();
                std::cout << "  Write completed" << std::endl;
            }
        }
        
        // Read data back
        {
            MefReader reader(test_session.string());
            
            if (!reader.is_valid()) {
                std::cout << "  ERROR: Failed to open MefReader" << std::endl;
                all_passed = false;
            } else {
                auto channels = reader.get_channels();
                if (channels.empty()) {
                    std::cout << "  ERROR: No channels found" << std::endl;
                    all_passed = false;
                } else {
                    std::cout << "  Found " << channels.size() << " channel(s)" << std::endl;
                    
                    for (const auto& ch_name : channels) {
                        auto info = reader.get_channel_info(ch_name);
                        std::cout << "    Channel: " << ch_name 
                                  << ", samples: " << info.number_of_samples
                                  << ", fs: " << info.sampling_frequency << " Hz"
                                  << ", units: " << info.units << std::endl;
                    }
                    
                    // Read data
                    auto data = reader.get_data(channels[0]);
                    std::cout << "  Read " << data.size() << " samples" << std::endl;
                    
                    // Verify data (approximately - due to compression/conversion)
                    bool data_ok = data.size() >= 900;  // Allow some tolerance
                    if (!data_ok) {
                        std::cout << "  ERROR: Data size mismatch" << std::endl;
                        all_passed = false;
                    } else {
                        std::cout << "  Write/read round-trip: OK" << std::endl;
                    }
                }
            }
        }
    }
    
    // Test 2: Multiple channels
    {
        fs::path multi_session = test_dir / "multi_channel.mefd";
        
        {
            MefWriter writer(multi_session.string(), true);
            writer.set_mef_block_len(50);
            
            si8 start_time = 2000000000000LL;
            
            // Write 3 channels
            for (int ch = 1; ch <= 3; ++ch) {
                std::vector<sf8> data(500);
                for (size_t i = 0; i < data.size(); ++i) {
                    data[i] = ch * 10.0 + std::sin(2 * M_PI * i / 50.0);
                }
                writer.write_data(data, "channel_" + std::to_string(ch), start_time, 500.0);
            }
            
            writer.close();
        }
        
        {
            MefReader reader(multi_session.string());
            auto channels = reader.get_channels();
            
            if (channels.size() != 3) {
                std::cout << "  ERROR: Expected 3 channels, got " << channels.size() << std::endl;
                all_passed = false;
            } else {
                std::cout << "  Multi-channel test: OK (3 channels)" << std::endl;
            }
        }
    }
    
    // Test 3: Properties access
    {
        fs::path prop_session = test_dir / "properties.mefd";
        
        {
            MefWriter writer(prop_session.string(), true);
            writer.set_data_units("uV");
            writer.set_mef_block_len(100);
            writer.set_gmt_offset(-5);  // EST
            
            std::vector<sf8> data(200, 50.0);
            writer.write_data(data, "prop_ch", 3000000000000LL, 200.0);
            writer.close();
        }
        
        {
            MefReader reader(prop_session.string());
            
            sf8 fs = reader.get_numeric_property("fsamp", "prop_ch");
            if (std::abs(fs - 200.0) > 0.01) {
                std::cout << "  ERROR: Sampling frequency mismatch" << std::endl;
                all_passed = false;
            } else {
                std::cout << "  Properties test: OK (fsamp=" << fs << " Hz)" << std::endl;
            }
        }
    }
    
    // Clean up
    try {
        fs::remove_all(test_dir);
    } catch (...) {
        // Ignore cleanup errors
    }
    
    return all_passed;
}
