#pragma once
#include <imgui/imgui.h>
#include <functional>
#include <string>

namespace ImStyleEd {

struct Element {
    std::string                 name{};
    std::function<void(ImVec4)> apply_color{};
    std::string                 description{}; // Optional description that will be displayed in the big config UI where you assign each element to a category / group.
};

} // namespace ImStyleEd