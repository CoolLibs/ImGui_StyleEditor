#include "ColorElement.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

void ColorElement::apply_to(ImGuiStyle& style) const
{
    style.Colors[_id] = _color; // TODO(JF) Apply the _brightness_change
}

void ColorElement::set_from_style(ImGuiStyle const& style)
{
    _color = style.Colors[_id];
}

void ColorElement::widget()
{
    ImGui::ColorEdit4(
        name(), (float*)&_color,
        ImGuiColorEditFlags_None
            // |ImGuiColorEditFlags_InputHSV
            | ImGuiColorEditFlags_NoInputs

    );
}

} // namespace ImStyleEd