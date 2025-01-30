#pragma once
#include <functional>
#include <iostream>
#include <string_view>

namespace ImStyleEd {

struct ErrorHandlers {
    std::function<void(std::string_view)> on_optional_warning = [](std::string_view error_message) {
        std::cout << error_message << '\n';
    };
    std::function<void(std::string_view)> on_mandatory_warning = [](std::string_view error_message) {
        std::cout << error_message << '\n';
    };
};

inline auto error_handlers() -> ErrorHandlers&
{
    static auto instance = ErrorHandlers{};
    return instance;
}

} // namespace ImStyleEd