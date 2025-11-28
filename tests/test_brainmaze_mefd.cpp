/**
 * @file test_brainmaze_mefd.cpp
 * @brief Unit tests for the brainmaze_mefd library
 */

#include <gtest/gtest.h>
#include "brainmaze_mefd.hpp"

namespace brainmaze_mefd {
namespace testing {

TEST(BrainmazeMefdTest, VersionReturnsNonNull) {
    const char* ver = version();
    ASSERT_NE(ver, nullptr);
}

TEST(BrainmazeMefdTest, VersionReturnsExpectedFormat) {
    const char* ver = version();
    EXPECT_STREQ(ver, "0.1.0");
}

} // namespace testing
} // namespace brainmaze_mefd
