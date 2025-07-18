#pragma once

#include <concepts>
#include <expected>
#include <format>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "types.hpp"

namespace stdx::details {

template <class T>
concept StringLike = std::is_convertible_v<std::remove_cv_t<T>, std::string_view>;

template <typename T>
    requires std::integral<T> || std::floating_point<T>
std::expected<T, scan_error> parse_value(std::string_view input) {
    std::remove_cv_t<T> result;
    auto err = std::from_chars(input.begin(), input.end(), result).ec;

    if (err == std::errc()) {
        return result;
    }

    std::error_code ec(static_cast<int>(err), std::generic_category());
    scan_error se;

    if (err == std::errc::invalid_argument) {
        se.message = std::format("{} - \"{}\" this is not a number for \"{}\"", ec.message(), input, typeid(T).name());
    } else if (err == std::errc::result_out_of_range) {
        se.message = std::format("{} - \"{}\" is out of range for \"{}\"", ec.message(), input, typeid(T).name());
    } else {
        se.message = std::format("{} - \"{}\" on parse to \"{}\"", ec.message(), input, typeid(T).name());
    }

    return std::unexpected(std::move(se));
}

template <typename T>
    requires StringLike<T>
std::expected<T, scan_error> parse_value(std::string_view input) {
    static_assert(!std::is_volatile_v<T>,
                  "Not support volatile qualification for string like types");  // Оно вообще нужно? В этом есть смысл?
    return T(input);
}

template <typename T>
consteval std::string_view type_to_fmt() {
    if constexpr (std::signed_integral<T>) {
        return "%d";
    } else if constexpr (std::unsigned_integral<T>) {
        return "%u";
    } else if constexpr (std::floating_point<T>) {
        return "%f";
    } else if constexpr (StringLike<T>) {
        return "%s";
    } else {
        static_assert(false, "Not supported type");
    }
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    if (!fmt.empty() && fmt != type_to_fmt<T>()) {
        return std::unexpected(
            scan_error{std::format("Incorrect format specified - \"{}\", expected - \"{}\" for type \"{}\"", fmt,
                                   type_to_fmt<T>(), typeid(T).name())});
    }

    return parse_value<T>(input);
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и текущей { есть текст,
        // проверяем его наличие во входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    // Проверяем оставшийся текст после последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

}  // namespace stdx::details