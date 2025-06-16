#pragma once

#include <string>

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {
    std::tuple<Ts...> scan_values;
    const std::tuple<Ts...> &values(void) { return scan_values; }
};

}  // namespace stdx::details
