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
    auto os = std::ofstream{_paths.config};
    {
        auto archive = cereal::JSONOutputArchive{os};
        archive(cereal::make_nvp("Config", _config));
    }
}

void Editor::load_config()
{
    auto is = std::ifstream{_paths.config};
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
    auto os = std::ofstream{_paths.themes};
    {
        auto archive = cereal::JSONOutputArchive{os};
        archive(
            cereal::make_nvp("Themes", _themes)
        );
    }
}

void Editor::load_themes()
{
    auto is = std::ifstream{_paths.themes};
    if (!is.is_open())
        return;
    try
    {
        auto archive = cereal::JSONInputArchive{is};
        archive(
            _themes
        );
    }
    catch (...)
    {
    }
}

void Editor::save_current_theme()
{
    remove_unknown_categories_from_theme(_current_theme);
    auto os = std::ofstream{_paths.current_theme};
    {
        auto archive = cereal::JSONOutputArchive{os};
        archive(
            cereal::make_nvp("Current theme", _current_theme)
        );
    }
}

void Editor::load_current_theme()
{
    auto is = std::ifstream{_paths.current_theme};
    if (!is.is_open())
        return;
    try
    {
        auto archive = cereal::JSONInputArchive{is};
        archive(
            _current_theme
        );
    }
    catch (...)
    {
    }
}

void Editor::remove_unknown_categories_from_theme(Theme& theme) const
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
    for (auto& theme : _themes)
        remove_unknown_categories_from_theme(theme);
}

void Editor::rename_category_in_themes(std::string const& old_category_name, std::string const& new_category_name)
{
    _current_theme.rename_category(old_category_name, new_category_name);
    for (auto& theme : _themes)
        theme.rename_category(old_category_name, new_category_name);
}

auto Editor::imgui_config_editor() -> bool
{
    auto const after_category_renamed = [&](std::string const& old_category_name, std::string const& new_category_name) {
        rename_category_in_themes(old_category_name, new_category_name);
        save_themes();        // It is important that this is done after category has been renamed, otherwise it will get removed when we try to remove the unknown categories.
        save_current_theme(); // Same
    };
    if (_config.imgui(after_category_renamed))
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
        save_current_theme();
    }

    return b;
}

auto Editor::imgui_theme_selector() -> bool
{
    bool b = false;

    if (ImGui::BeginCombo("Theme", _current_theme.name().c_str()))
    {
        // ColorTheme const* theme_to_delete = nullptr;
        for (auto const& theme : _themes)
        {
            ImGui::PushID(&theme);
            if (ImGui::Selectable(theme.name().c_str()))
            {
                _current_theme = theme;
                save_current_theme();
                b = true;
            }
            // if (ImGui::BeginPopupContextItem("##ctx"))
            // {
            //     if (ImGui::Button("Delete theme (This can't be undone!)"))
            //         theme_to_delete = &theme;
            //     ImGui::EndPopup();
            // }
            ImGui::PopID();
        }
        // if (theme_to_delete)
        // {
        //     if (theme_to_delete->name == _current_theme.name)
        //     {
        //         _current_theme.name = "";
        //     }
        //     _themes.erase(std::remove_if(_themes.begin(), _themes.end(), [&](ColorTheme const& theme) { return &theme == theme_to_delete; }), _themes.end());
        //     save_to_disk();
        // }
        ImGui::EndCombo();
    }

    return b;
}

} // namespace ImStyleEd