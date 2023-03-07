#pragma once
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

    void apply(ImGuiStyle&);

    auto name() -> auto& { return _name; }
    auto name() const -> auto const& { return _name; }
    auto elements() -> auto& { return _elements; }
    auto elements() const -> auto const& { return _elements; }

private:
    std::string               _name{"Give me a name by clicking HERE"};
    ImVec4                    _color{};
    std::vector<ColorElement> _elements;
};

auto category_with_all_color_elements() -> ColorCategory;

} // namespace ImStyleEd