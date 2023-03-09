#pragma once
#include <array>
#include <string>
#include <vector>
#include "ColorElement.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

class ColorCategory {
public:
    explicit ColorCategory(std::vector<ColorElement> elements = {})
        : _elements{std::move(elements)}
    {}

    void add_element(ColorElement const&);
    void remove_element(ImGuiCol);

    void apply_to(ImGuiStyle&) const;
    void set_from_style(ImGuiStyle const&);

    auto name() -> auto& { return _name; }
    auto name() const -> auto const& { return _name; }
    auto elements() -> auto& { return _elements; }
    auto elements() const -> auto const& { return _elements; }

private:
    std::string               _name{"Give me a name by clicking HERE"};
    std::array<float, 3>      _color{0.f, 0.f, 0.f}; // Not a vec4, the opacity is handled by each element separately
    std::vector<ColorElement> _elements;
};

auto category_with_all_color_elements() -> ColorCategory;

} // namespace ImStyleEd