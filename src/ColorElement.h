#pragma once
#include <string>
#include "imgui/imgui.h"

namespace ImStyleEd {

class ColorElement {
public:
    void apply(ImGuiStyle&, ImVec4 color) const;

    auto color() -> auto& { return _color; }
    auto color() const -> auto const& { return _color; }
    auto brightness_change() -> auto& { return _brightness_change; }
    auto brightness_change() const -> auto const& { return _brightness_change; }
    auto name() const -> const char* { return "e"; }

private:
    ImVec4   _color{};
    float    _brightness_change{0.f}; // 0 means using the same color as the category, > 0 means using a brighter one, and < 0 a darker one;
    ImGuiCol _id{};
};

} // namespace ImStyleEd