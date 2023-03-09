#include "ColorElement.hpp"
#include "color_conversions.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

void ColorElement::apply_to(ImGuiStyle& style) const
{
    style.Colors[_id] = _color;
}

static auto lerp(float a, float b, float t) -> float
{
    return a + t * (b - a);
}

void ColorElement::update_color(std::array<float, 3> const& color)
{
    auto cielab = CIELAB_from_sRGB({color[0], color[1], color[2]});
    if (_brightness_change > 0.f)
        cielab.x = lerp(cielab.x, 1.f, _brightness_change);
    else
        cielab.x = lerp(cielab.x, 0.f, -_brightness_change);

    auto srgb = sRGB_from_CIELAB(cielab);

    _color.x = srgb.x;
    _color.y = srgb.y;
    _color.z = srgb.z;
}

void ColorElement::set_from_style(ImGuiStyle const& style)
{
    _color = style.Colors[_id];
}

auto ColorElement::widget() -> bool
{
    ImGui::ColorEdit4(
        name(), (float*)&_color,
        ImGuiColorEditFlags_None
            // |ImGuiColorEditFlags_InputHSV
            | ImGuiColorEditFlags_NoInputs
            // | ImGuiColorEditFlags_NoPicker
            | ImGuiColorEditFlags_AlphaPreview
    );
    bool b = false;
    if (ImGui::BeginPopupContextItem(std::to_string(_id).c_str()))
    {
        b |= ImGui::SliderFloat("Opacity", &_color.w, 0.f, 1.f);
        b |= ImGui::SliderFloat("Brightness Delta", &_brightness_change, -1.f, 1.f);

        ImGui::EndPopup();
    }
    return b;
}

} // namespace ImStyleEd