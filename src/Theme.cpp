#include "Theme.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

auto Theme::color_for_category(std::string const& category_name) const -> Color
{
    auto const it = categories_colors.find(category_name);
    if (it == categories_colors.end())
        return Color{1.f, 0.f, 1.f};

    return it->second;
}

auto Theme::imgui(ForEachCategoryName const& for_each_category_name) -> bool
{
    bool b = false;
    b |= ImGui::Checkbox("Is Dark Mode", &is_dark_mode);
    for_each_category_name([&](std::string const& category_name) {
        auto& color = categories_colors[category_name]; // Will create a default color if the category was not known yet.
        b |= ImGui::ColorEdit3(category_name.c_str(), &color.r, ImGuiColorEditFlags_NoInputs);
    });
    return b;
}

void Theme::rename_category(std::string const& old_category_name, std::string const& new_category_name)
{
    categories_colors[new_category_name] = categories_colors[old_category_name];
    categories_colors.erase(old_category_name);
}

} // namespace ImStyleEd