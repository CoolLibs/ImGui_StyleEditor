#include "ColorCategory.h"
#include "imgui/imgui.h"

namespace ImStyleEd {

void ColorCategory::apply(ImGuiStyle& style)
{
    for (auto& element : _elements)
    {
        element.apply(style, _color);
    }
}

} // namespace ImStyleEd