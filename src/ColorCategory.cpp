#include "ColorCategory.hpp"
#include <algorithm>
#include "imgui/imgui.h"

namespace ImStyleEd {

void ColorCategory::apply_to(ImGuiStyle& style) const
{
    for (auto const& group : _brightness_groups)
        group.apply_to(style);
}

void ColorCategory::set_from_style(ImGuiStyle const& style)
{
    for (auto& group : _brightness_groups)
        group.set_from_style(style);
}

void ColorCategory::update_colors(bool is_dark_mode)
{
    for (auto& group : _brightness_groups)
        group.update_color(_color, is_dark_mode);
}

void ColorCategory::sort()
{
    std::sort(_brightness_groups.begin(), _brightness_groups.end(), [](BrightnessGroup const& a, BrightnessGroup const& b) {
        return a.brightness_level() < b.brightness_level();
    });
}

auto category_with_all_color_elements() -> ColorCategory
{
    return ColorCategory{std::vector<BrightnessGroup>{std::vector<ImGuiCol>{
#include "generated/list_all_color_elements.inl"
    }}};
}

auto ColorCategory::widget(bool is_dark_mode) -> bool
{
    bool b = ImGui::ColorEdit3(name().c_str(), color().data(), ImGuiColorEditFlags_NoInputs);
    if (b)
    {
        for (auto& group : _brightness_groups)
            group.update_color(_color, is_dark_mode);
    }
    return b;
}

void ColorCategory::add_brightness_group()
{
    _brightness_groups.emplace_back();
}

} // namespace ImStyleEd