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
        , _opacity{color.w}
    {}

    void apply_to(ImGuiStyle&) const;
    void widget();
    void set_from_style(ImGuiStyle const&);

    auto color() -> auto& { return _color; }
    auto color() const -> auto const& { return _color; }
    auto brightness_change() -> auto& { return _brightness_change; }
    auto brightness_change() const -> auto const& { return _brightness_change; }
    auto name() const -> const char* { return color_id_to_string(_id); }
    auto id() const -> ImGuiCol { return _id; }

private:
    ImGuiCol _id{};
    ImVec4   _color{};                // TODO(JF) Color should be computed based on the category, the _brightness_change and the _opacity.
    float    _brightness_change{0.f}; // 0 means using the same color as the category, > 0 means using a brighter one, and < 0 a darker one;
    float    _opacity{1.f};
};

} // namespace ImStyleEd