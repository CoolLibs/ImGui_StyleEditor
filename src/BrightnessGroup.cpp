#include "BrightnessGroup.hpp"
#include <algorithm>
#include "color_conversions.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

void BrightnessGroup::add_element(ImGuiCol id, sRGBColor const& color, bool is_dark_mode)
{
    _ids.push_back(id);
    sort();
    update_color(color, is_dark_mode);
    apply_to(ImGui::GetStyle());
}

void BrightnessGroup::remove_element(ImGuiCol id)
{
    _ids.erase(std::remove(_ids.begin(), _ids.end(), id), _ids.end());
    sort();
}

void BrightnessGroup::sort()
{
    std::sort(_ids.begin(), _ids.end(), [](ImGuiCol const& a, ImGuiCol const& b) {
        return std::string{color_id_to_string(a)} < std::string{color_id_to_string(b)};
    });
}

void BrightnessGroup::apply_to(ImGuiStyle& style) const
{
    for (auto const& id : _ids)
        style.Colors[id] = _color;
}

static auto lerp(float a, float b, float t) -> float
{
    return a + t * (b - a);
}

void BrightnessGroup::update_color(sRGBColor const& color, bool is_dark_mode)
{
    auto cielab = CIELAB_from_sRGB({color[0], color[1], color[2]});
    if (_brightness_level > 0.f)
        cielab.x = lerp(cielab.x, is_dark_mode ? 1.f : 0.f, _brightness_level);
    else
        cielab.x = lerp(cielab.x, is_dark_mode ? 0.f : 1.f, -_brightness_level);

    auto srgb = sRGB_from_CIELAB(cielab);

    _color.x = srgb.x;
    _color.y = srgb.y;
    _color.z = srgb.z;
}

auto BrightnessGroup::name() const -> std::string
{
    return std::string{"Brightness: "}
           + (_brightness_level < 0.f ? "-" : "+")
           + std::to_string(static_cast<int>(std::round(std::abs(_brightness_level) * 100.f)))
           + "%";
}

auto BrightnessGroup::widget() -> bool
{
    ImGui::PushID(this);
    bool b = false;
    b |= ImGui::SliderFloat("Brightness", &_brightness_level, -1.f, 1.f);
    b |= ImGui::SliderFloat("Opacity", &_color.w, 0.f, 1.f);

    // if (ImGui::BeginChild(std::to_string((int)this).c_str(), {}, true))
    // {
    for (auto const id : _ids)
    {
        ImGui::ColorEdit4(
            color_id_to_string(id), (float*)&ImGui::GetStyle().Colors[id],
            ImGuiColorEditFlags_None
                // |ImGuiColorEditFlags_InputHSV
                | ImGuiColorEditFlags_NoInputs
                // | ImGuiColorEditFlags_NoPicker
                | ImGuiColorEditFlags_AlphaPreview
        );
        b |= ImGui::IsItemActive();

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            // Set payload to carry the index of our item (could be anything)
            ImGui::SetDragDropPayload("DND_DEMO_CELL", &id, sizeof(ImGuiCol));

            // Display preview (could be anything, e.g. when dragging an image we could decide to display
            // the filename and a small preview of the image, etc.)

            ImGui::TextUnformatted(color_id_to_string(id));

            ImGui::EndDragDropSource();
        }
    }
    // }
    // ImGui::EndChild();
    ImGui::PopID();

    return b;
}

} // namespace ImStyleEd