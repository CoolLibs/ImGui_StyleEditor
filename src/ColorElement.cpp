#include "ColorElement.hpp"

namespace ImStyleEd {

void ColorElement::apply(ImGuiStyle& style, ImVec4 color) const
{
    style.Colors[_id] = color; // TODO(JF) Apply the _brightness_change
}

} // namespace ImStyleEd