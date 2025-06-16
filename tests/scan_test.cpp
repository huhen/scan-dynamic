#include <gtest/gtest.h>
#include <print>
#include <string>
#include <string_view>

#include "scan.hpp"

// Неподдерживаемы типы просто не скомпилируются

TEST(ScanTest, AllSupportedFmtTypes) {
    auto result = stdx::scan<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double,
                             std::string_view, std::string>(
        "-8, -16, -32, -64, 8, 16, 32, 64, 3.14, 1.4142135623730950488016887242097, string_view, string.",
        "{%d}, {%d}, {%d}, {%d}, {%u}, {%u}, {%u}, {%u}, {%f}, {%f}, {%s}, {%s}.");
    if (result.has_value()) {
        EXPECT_EQ(result->get<0>(), -8);
        EXPECT_EQ(result->get<1>(), -16);
        EXPECT_EQ(result->get<2>(), -32);
        EXPECT_EQ(result->get<3>(), -64);
        EXPECT_EQ(result->get<4>(), 8);
        EXPECT_EQ(result->get<5>(), 16);
        EXPECT_EQ(result->get<6>(), 32);
        EXPECT_EQ(result->get<7>(), 64);
        EXPECT_FLOAT_EQ(result->get<8>(), 3.14f);
        EXPECT_DOUBLE_EQ(result->get<9>(), 1.4142135623730950488016887242097);
        EXPECT_EQ(result->get<10>(), "string_view");
        EXPECT_EQ(result->get<11>(), "string");
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, AllSupportedTypes) {
    auto result = stdx::scan<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double,
                             std::string_view, std::string>(
        "-8, -16, -32, -64, 8, 16, 32, 64, 3.14, 1.4142135623730950488016887242097, string_view, string.",
        "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}.");
    if (result.has_value()) {
        EXPECT_EQ(result->get<0>(), -8);
        EXPECT_EQ(result->get<1>(), -16);
        EXPECT_EQ(result->get<2>(), -32);
        EXPECT_EQ(result->get<3>(), -64);
        EXPECT_EQ(result->get<4>(), 8);
        EXPECT_EQ(result->get<5>(), 16);
        EXPECT_EQ(result->get<6>(), 32);
        EXPECT_EQ(result->get<7>(), 64);
        EXPECT_FLOAT_EQ(result->get<8>(), 3.14f);
        EXPECT_DOUBLE_EQ(result->get<9>(), 1.4142135623730950488016887242097);
        EXPECT_EQ(result->get<10>(), "string_view");
        EXPECT_EQ(result->get<11>(), "string");
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, AllSupportedConstTypes) {
    auto result = stdx::scan<const int8_t, const int16_t, const int32_t, const int64_t, const uint8_t, const uint16_t,
                             const uint32_t, const uint64_t, const float, const double, const std::string_view,
                             const std::string>(
        "-8, -16, -32, -64, 8, 16, 32, 64, 3.14, 1.4142135623730950488016887242097, string_view, string.",
        "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}.");
    if (result.has_value()) {
        EXPECT_EQ(result->get<0>(), -8);
        EXPECT_EQ(result->get<1>(), -16);
        EXPECT_EQ(result->get<2>(), -32);
        EXPECT_EQ(result->get<3>(), -64);
        EXPECT_EQ(result->get<4>(), 8);
        EXPECT_EQ(result->get<5>(), 16);
        EXPECT_EQ(result->get<6>(), 32);
        EXPECT_EQ(result->get<7>(), 64);
        EXPECT_FLOAT_EQ(result->get<8>(), 3.14f);
        EXPECT_DOUBLE_EQ(result->get<9>(), 1.4142135623730950488016887242097);
        EXPECT_EQ(result->get<10>(), "string_view");
        EXPECT_EQ(result->get<11>(), "string");
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, AllSupportedVolatileTypes) {
    auto result = stdx::scan<volatile int8_t, volatile int16_t, volatile int32_t, volatile int64_t, volatile uint8_t,
                             volatile uint16_t, volatile uint32_t, volatile uint64_t, volatile float, volatile double,
                             std::string_view, std::string>(
        "-8, -16, -32, -64, 8, 16, 32, 64, 3.14, 1.4142135623730950488016887242097, string_view, string.",
        "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}.");
    if (result.has_value()) {
        EXPECT_EQ(result->get<0>(), -8);
        EXPECT_EQ(result->get<1>(), -16);
        EXPECT_EQ(result->get<2>(), -32);
        EXPECT_EQ(result->get<3>(), -64);
        EXPECT_EQ(result->get<4>(), 8);
        EXPECT_EQ(result->get<5>(), 16);
        EXPECT_EQ(result->get<6>(), 32);
        EXPECT_EQ(result->get<7>(), 64);
        EXPECT_FLOAT_EQ(result->get<8>(), 3.14f);
        EXPECT_DOUBLE_EQ(result->get<9>(), 1.4142135623730950488016887242097);
        EXPECT_EQ(result->get<10>(), "string_view");
        EXPECT_EQ(result->get<11>(), "string");
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, NotMatchParamsAndPlaceholders) {
    auto result = stdx::scan<int>("I want to sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    if (!result.has_value()) {
        EXPECT_EQ(result.error().message, "The number of placeholders(2) does not match the number of parameters(1)");
    } else
        FAIL() << "This shouldn't go well.";
}
