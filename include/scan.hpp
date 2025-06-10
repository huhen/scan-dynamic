#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <format>
#include <tuple>

namespace stdx {

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {
    const auto string_values = details::parse_sources<Ts...>(input, format);

    if (!string_values.has_value()) {
        return std::unexpected(std::move(string_values.error()));
    }

    const auto &formats = string_values->first;
    const auto &inputs = string_values->second;

    if (formats.size() != inputs.size()) {
        return std::unexpected{details::scan_error{std::format(
            "The number of placeholders({}) does not match the number of inputs({})", formats.size(), inputs.size())}};
    }

    if (formats.size() != sizeof...(Ts)) {
        return std::unexpected{details::scan_error{
            std::format("The number of placeholders({}) does not match the number of parameters({})", formats.size(),
                        sizeof...(Ts))}};
    }

    auto values = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(details::parse_value_with_format<Ts>(inputs[Is], formats[Is])...);
    }(std::index_sequence_for<Ts...>{});

    std::optional<std::string> combined_error;

    auto check_error = [&](const auto &item) {
        if (!item.has_value()) {
            if (combined_error.has_value()) {
                *combined_error += "; " + item.error().message;
            } else {
                combined_error = std::move(item.error().message);
            }
        }
    };
    std::apply([&](const auto &...items) { (check_error(items), ...); }, values);

    if (combined_error.has_value()) {
        return std::unexpected(details::scan_error{std::move(*combined_error)});
    }

    return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> details::scan_result<Ts...> {
        return {std::move(std::get<Is>(values)).value()...};
    }(std::index_sequence_for<Ts...>{});
}

}  // namespace stdx
