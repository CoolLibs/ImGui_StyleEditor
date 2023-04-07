#include "Theme.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

auto Theme::color_for_category(std::string const& category_name) -> Color
{
    auto const it = _categories_colors.find(category_name);
    if (it == _categories_colors.end())
        return Color{1.f, 0.f, 1.f};

    return it->second;
}

auto Theme::imgui(ForEachCategoryName const& for_each_category_name) -> bool
{
    bool b = false;
    b |= ImGui::Checkbox("Is Dark Mode", &_is_dark_mode);
    for_each_category_name([&](std::string const& category_name) {
        auto& color = _categories_colors[category_name]; // Will create a default color if the category was not known yet.
        b |= ImGui::ColorEdit3(category_name.c_str(), &color.r);
    });
    return b;
}

} // namespace ImStyleEd