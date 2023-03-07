#include "ColorCategory.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

void ColorCategory::apply(ImGuiStyle& style)
{
    for (auto& element : _elements)
    {
        element.apply(style, _color);
    }
}

void ColorCategory::add_element(ColorElement const& element)
{
    _elements.push_back(element);
}

void ColorCategory::remove_element(ImGuiCol id)
{
    _elements.erase(std::remove_if(_elements.begin(), _elements.end(), [&](ColorElement const& elem) { return elem.id() == id; }), _elements.end());
}

auto category_with_all_color_elements() -> ColorCategory
{
    return ColorCategory{std::vector<ColorElement>{
#include "generated/list_all_color_elements.inl"
    }};
}

} // namespace ImStyleEd