#pragma once
#include <imgui/imgui.h>
#include <functional>
#include <string>

namespace ImStyleEd {

struct ColorElement {
    std::string                 name{};
    std::function<void(ImVec4)> apply_color{};
};

} // namespace ImStyleEd