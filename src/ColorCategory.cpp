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

auto category_with_all_color_elements() -> ColorCategory
{
    return ColorCategory{std::vector<ColorElement>{
#include "generated/list_all_color_elements.inl"
    }};
}

} // namespace ImStyleEd