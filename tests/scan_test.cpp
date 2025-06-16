#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
    auto result =
        stdx::scan<std::string, float>("I want to sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    ASSERT_FALSE(result);
}