#include "ColorElement.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

void ColorElement::apply_to(ImGuiStyle& style) const
{
    style.Colors[_id] = _color;
}

void ColorElement::update_color(std::array<float, 3> const& color)
{
    _color.x = color[0];
    _color.y = color[1];
    _color.z = color[2];
    _color.w = _opacity;
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