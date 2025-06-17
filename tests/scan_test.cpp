#include "scan.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <gtest/gtest.h>
#include <print>
#include <string>
#include <string_view>
#include <tuple>

// Неподдерживаемы типы просто не скомпилируются

static bool starts_with(std::string_view str, std::string_view prefix) {
    return str.substr(0, prefix.size()) == prefix;
}

#define EXPECT_STARTS_WITH(str, prefix) EXPECT_TRUE(starts_with(str, prefix))

template <typename T>
struct scan_helper;

template <typename... Ts>
struct scan_helper<std::tuple<Ts...>> {
    static auto scan(std::string_view input, std::string_view format) { return stdx::scan<Ts...>(input, format); }
};

template <std::size_t I, typename T1, typename T2>
constexpr void compare_tuple_element(const T1 &result, const T2 &values) {
    using Type = std::tuple_element_t<I, T1>;

    const auto &actual = std::get<I>(result);
    const auto &expected = std::get<I>(values);

    if constexpr (std::floating_point<Type>) {
        if (std::isnan(actual) && std::isnan(expected)) {
            SUCCEED();
        } else {
            if constexpr (sizeof(Type) == sizeof(float)) {
                EXPECT_FLOAT_EQ(actual, expected);
            } else {
                EXPECT_DOUBLE_EQ(actual, expected);
            }
        }
    } else
        EXPECT_EQ(actual, expected);
}

template <typename Tuple1, typename Tuple2>
constexpr void compare_tuples(const Tuple1 &result, const Tuple2 &values) {
    static_assert(std::tuple_size_v<Tuple1> == std::tuple_size_v<Tuple2>, "Tuples length must be equal");
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (compare_tuple_element<Is>(result, values), ...);
    }(std::make_index_sequence<std::tuple_size_v<Tuple1>>{});
}

TEST(ScanTest, AllSupportedFmtTypes) {
    std::tuple values{static_cast<int8_t>(-8),         static_cast<int16_t>(-16),
                      static_cast<int32_t>(-32),       static_cast<int64_t>(-64),
                      static_cast<uint8_t>(8),         static_cast<uint16_t>(16),
                      static_cast<uint32_t>(32),       static_cast<uint64_t>(64),
                      static_cast<float>(3.14f),       static_cast<double>(1.4142135623730950488016887242097),
                      std::string_view("string_view"), std::string("string")};

    constexpr std::string_view format = "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}.";
    const std::string input = std::apply([&](auto &&...args) { return std::format(format, args...); }, values);

    const auto result = scan_helper<decltype(values)>::scan(
        input, "{%d}, {%d}, {%d}, {%d}, {%u}, {%u}, {%u}, {%u}, {%f}, {%f}, {%s}, {%s}.");

    if (result.has_value()) {
        compare_tuples(result->values(), values);
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, AllSupportedTypes) {
    const std::string_view sw("string_view");
    const std::string s("string");

    std::tuple values{static_cast<int8_t>(-8),
                      static_cast<int16_t>(-16),
                      static_cast<int32_t>(-32),
                      static_cast<int64_t>(-64),
                      static_cast<uint8_t>(8),
                      static_cast<uint16_t>(16),
                      static_cast<uint32_t>(32),
                      static_cast<uint64_t>(64),
                      static_cast<float>(3.14f),
                      static_cast<double>(1.4142135623730950488016887242097),
                      sw,
                      s};

    constexpr std::string_view format = "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}.";
    const std::string input = std::apply([&](auto &&...args) { return std::format(format, args...); }, values);

    const auto result = scan_helper<decltype(values)>::scan(input, format);

    if (result.has_value()) {
        compare_tuples(result->values(), values);
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, AllSupportedConstTypes) {
    std::tuple values{static_cast<const int8_t>(-8),   static_cast<const int16_t>(-16),
                      static_cast<const int32_t>(-32), static_cast<const int64_t>(-64),
                      static_cast<const uint8_t>(8),   static_cast<const uint16_t>(16),
                      static_cast<const uint32_t>(32), static_cast<const uint64_t>(64),
                      static_cast<const float>(3.14f), static_cast<const double>(1.4142135623730950488016887242097),
                      std::string_view("string_view"), std::string("string")};

    constexpr std::string_view format = "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}.";
    const std::string input = std::apply([&](auto &&...args) { return std::format(format, args...); }, values);

    const auto result = scan_helper<decltype(values)>::scan(input, format);

    if (result.has_value()) {
        compare_tuples(result->values(), values);
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, AllSupportedVolatileTypes) {
    std::tuple values{
        static_cast<volatile int8_t>(-8),   static_cast<volatile int16_t>(-16),
        static_cast<volatile int32_t>(-32), static_cast<volatile int64_t>(-64),
        static_cast<volatile uint8_t>(8),   static_cast<volatile uint16_t>(16),
        static_cast<volatile uint32_t>(32), static_cast<volatile uint64_t>(64),
        static_cast<volatile float>(3.14f), static_cast<volatile double>(1.4142135623730950488016887242097),
        std::string_view("string_view"),    std::string("string")};

    constexpr std::string_view format = "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}.";
    const std::string input = std::apply([&](auto &&...args) { return std::format(format, args...); }, values);

    const auto result = scan_helper<decltype(values)>::scan(input, format);

    if (result.has_value()) {
        compare_tuples(result->values(), values);
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, MinMaxValues) {
    std::tuple values{std::numeric_limits<float>::max(),    std::numeric_limits<double>::max(),
                      std::numeric_limits<float>::min(),    std::numeric_limits<double>::min(),
                      std::numeric_limits<int8_t>::max(),   std::numeric_limits<int16_t>::max(),
                      std::numeric_limits<int32_t>::max(),  std::numeric_limits<int64_t>::max(),
                      std::numeric_limits<uint8_t>::max(),  std::numeric_limits<uint16_t>::max(),
                      std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint64_t>::max(),
                      std::numeric_limits<int8_t>::min(),   std::numeric_limits<int16_t>::min(),
                      std::numeric_limits<int32_t>::min(),  std::numeric_limits<int64_t>::min(),
                      std::numeric_limits<uint8_t>::min(),  std::numeric_limits<uint16_t>::min(),
                      std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint64_t>::min()};

    constexpr std::string_view format =
        "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}.";
    const std::string input = std::apply([&](auto &&...args) { return std::format(format, args...); }, values);

    const auto result = scan_helper<decltype(values)>::scan(input, format);

    if (result.has_value()) {
        compare_tuples(result->values(), values);
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, SpecValues) {
    std::tuple values{std::numeric_limits<float>::infinity(),  std::numeric_limits<double>::infinity(),
                      -std::numeric_limits<float>::infinity(), -std::numeric_limits<double>::infinity(),
                      std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};

    constexpr std::string_view format = "{}, {}, {}, {}, {}, {}.";
    const std::string input = std::apply([&](auto &&...args) { return std::format(format, args...); }, values);

    const auto result = scan_helper<decltype(values)>::scan(input, format);

    if (result.has_value()) {
        compare_tuples(result->values(), values);
    } else
        FAIL() << result.error().message;
}

TEST(ScanTest, NotMatchParamsAndPlaceholders) {
    auto result = stdx::scan<int>("I want to sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    if (!result.has_value()) {
        EXPECT_EQ(result.error().message, "The number of placeholders(2) does not match the number of parameters(1)")
            << result.error().message;
    } else
        FAIL() << "This shouldn't go well.";
}

TEST(ScanTest, NotMatchFormatAndInput) {
    auto result =
        stdx::scan<int, float>("I want to BAD sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    if (!result.has_value()) {
        EXPECT_EQ(result.error().message, "Unformatted text in input and format string are different")
            << result.error().message;
    } else
        FAIL() << "This shouldn't go well.";
}

TEST(ScanTest, IncorrectFormat) {
    auto result = stdx::scan<int, int>("I want to sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    if (!result.has_value()) {
        EXPECT_STARTS_WITH(result.error().message, "Incorrect format specified") << result.error().message;
    } else
        FAIL() << "This shouldn't go well.";
}

TEST(ScanTest, IsNotNumber) {
    auto result = stdx::scan<int, float>("I want to sum BAD and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    if (!result.has_value()) {
        EXPECT_STARTS_WITH(result.error().message, "Invalid argument") << result.error().message;
    } else
        FAIL() << "This shouldn't go well.";
}

TEST(ScanTest, OutOfRange) {
    auto result =
        stdx::scan<int8_t, float>("I want to sum 128 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    if (!result.has_value()) {
        EXPECT_STARTS_WITH(result.error().message, "Numerical result out of range") << result.error().message;
    } else
        FAIL() << "This shouldn't go well.";
}
