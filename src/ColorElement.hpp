#pragma once
#include <string>
#include "color_id_to_string.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

class ColorElement {
public:
    explicit ColorElement(ImGuiCol id, ImVec4 color = {}, float brightness_change = 0.f)
        : _id{id}
        , _color{color}
        , _brightness_change{brightness_change}
    {}

    void apply(ImGuiStyle&, ImVec4 color) const;
    void widget();

    auto color() -> auto& { return _color; }
    auto color() const -> auto const& { return _color; }
    auto brightness_change() -> auto& { return _brightness_change; }
    auto brightness_change() const -> auto const& { return _brightness_change; }
    auto name() const -> const char* { return color_id_to_string(_id); }
    auto id() const -> ImGuiCol { return _id; }

private:
    ImGuiCol _id{};
    ImVec4   _color{};
    float    _brightness_change{0.f}; // 0 means using the same color as the category, > 0 means using a brighter one, and < 0 a darker one;
};

} // namespace ImStyleEd