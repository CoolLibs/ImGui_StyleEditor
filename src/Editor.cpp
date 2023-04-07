#include "Editor.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>
#include "color_conversions.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

static auto lerp(float a, float b, float t) -> float
{
    return a + t * (b - a);
}

static auto compute_color(Group const& group, Color const& category_color, bool is_dark_mode, bool behaves_diff_in_light)
    -> ImVec4
{
    auto cielab = CIELAB_from_sRGB({category_color.r, category_color.g, category_color.b});
    if (group.brightness_delta > 0.f)
        cielab.x = lerp(cielab.x, is_dark_mode || !behaves_diff_in_light ? 1.f : 0.f, group.brightness_delta);
    else
        cielab.x = lerp(cielab.x, is_dark_mode || !behaves_diff_in_light ? 0.f : 1.f, -group.brightness_delta);

    auto srgb = sRGB_from_CIELAB(cielab);

    return {srgb.r, srgb.g, srgb.b, group.opacity};
}

void Editor::apply()
{
    auto const elements_per_group = _config.elements_per_group();
    int        idx                = 0;
    for (auto const& category : _config.categories())
    {
        auto const category_color = _current_theme.color_for_category(category.name);
        for (auto const& group : category.groups)
        {
            auto const group_color = compute_color(group, category_color, _current_theme.is_dark_mode(), category.behaves_differently_in_light_mode);
            for (auto const& element : elements_per_group[idx])
            {
                element->first.set_color(group_color);
            }
            idx++;
        }
    }
}

void Editor::save_config()
{
    auto os = std::ofstream{_paths.config_path};
    {
        auto archive = cereal::JSONOutputArchive{os};
        archive(cereal::make_nvp("Config", _config));
    }
}

void Editor::load_config()
{
    auto is = std::ifstream{_paths.config_path};
    if (!is.is_open())
        return;
    try
    {
        auto archive = cereal::JSONInputArchive{is};
        archive(_config);
    }
    catch (...)
    {
    }
}

void Editor::save_themes()
{
    remove_unknown_categories_from_themes();
    auto os = std::ofstream{_paths.themes_path};
    {
        auto archive = cereal::JSONOutputArchive{os};
        archive(
            cereal::make_nvp("Current theme", _current_theme),
            cereal::make_nvp("Themes", _themes)
        );
    }
}

void Editor::load_themes()
{
    auto is = std::ifstream{_paths.themes_path};
    if (!is.is_open())
        return;
    try
    {
        auto archive = cereal::JSONInputArchive{is};
        archive(
            _current_theme,
            _themes
        );
    }
    catch (...)
    {
    }
}

void Editor::remove_unknown_categories_from_theme(Theme& theme)
{
    auto keep = std::unordered_map<std::string, Color>{};
    for (auto const& category : _config.categories())
    {
        keep[category.name] = theme.color_for_category(category.name);
    }
    theme.set_colors_for_categories(keep);
}

void Editor::remove_unknown_categories_from_themes()
{
    remove_unknown_categories_from_theme(_current_theme);
    for (auto& theme : _themes)
        remove_unknown_categories_from_theme(theme);
}

auto Editor::imgui_config_editor() -> bool
{
    if (_config.imgui())
    {
        apply();
        save_config();
        return true;
    }
    return false;
}

auto Editor::imgui_themes_editor() -> bool
{
    bool const b = _current_theme.imgui([&](std::function<void(std::string const&)> const& callback) {
        for (auto const& category : _config.categories())
        {
            callback(category.name);
        }
    });
    if (b)
    {
        apply();
        save_themes();
    }

    return b;
}

} // namespace ImStyleEd