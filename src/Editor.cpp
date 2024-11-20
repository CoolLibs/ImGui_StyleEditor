#include "Editor.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>
#include "color_conversions.hpp"
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

namespace ImStyleEd {

static auto lerp(float a, float b, float t) -> float
{
    return a + t * (b - a);
}

static auto compute_color(Group const& group, Color const& category_color, bool is_dark_mode, bool behaves_diff_in_light)
    -> ImVec4
{
    auto lab = Oklab_from_sRGB({category_color.r, category_color.g, category_color.b});
    if (group.brightness_delta > 0.f)
        lab.x = lerp(lab.x, is_dark_mode || !behaves_diff_in_light ? 1.f : 0.f, group.brightness_delta);
    else
        lab.x = lerp(lab.x, is_dark_mode || !behaves_diff_in_light ? 0.f : 1.f, -group.brightness_delta);

    auto srgb = sRGB_from_Oklab(lab);

    return {srgb.r, srgb.g, srgb.b, group.opacity};
}

void Editor::apply_current_theme()
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

void Editor::apply_theme_if_any(std::string_view theme_name)
{
    auto const* theme = try_get_theme(theme_name);
    if (!theme)
        return;
    _current_theme = *theme;
    apply_current_theme();
    save_current_theme();
}

auto Editor::try_get_theme(std::string_view theme_name) const -> Theme const*
{
    auto const it = std::find_if(_themes.begin(), _themes.end(), [&](Theme const& theme) {
        return theme.name() == theme_name;
    });
    if (it == _themes.end())
        return nullptr;
    return &*it;
}

auto Editor::get_color_from_theme_if_any(std::string_view theme_name, std::string_view color_category) const -> Color
{
    auto const* theme = try_get_theme(theme_name);
    if (!theme)
        return Color{1.f, 0.f, 1.f};
    return theme->color_for_category(std::string{color_category});
}

auto Editor::get_color(std::string_view color_category) const -> Color
{
    return _current_theme.color_for_category(std::string{color_category});
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

auto Editor::load_current_theme() -> bool
{
    auto is = std::ifstream{_paths.current_theme};
    if (!is.is_open())
        return false;
    try
    {
        auto archive = cereal::JSONInputArchive{is};
        archive(
            _current_theme
        );
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void Editor::add_current_theme_to_the_list_of_recorded_themes()
{
    _current_theme.set_name(_next_theme_name);
    save_current_theme();

    _themes.push_back(_current_theme);
    save_themes();

    _next_theme_name = "";
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
        apply_current_theme();
        save_config();
        return true;
    }
    return false;
}

auto Editor::imgui_themes_editor() -> bool
{
    bool b = false;

    // Theme selector
    b |= imgui_theme_selector(true /*is_allowed_to_delete_themes*/);

    // Edit current theme
    if (_current_theme.imgui([&](std::function<void(std::string const&)> const& callback) {
            for (auto const& category : _config.categories())
            {
                callback(category.name);
            }
        }))
    {
        _current_theme.set_name("");
        apply_current_theme();
        save_current_theme();
        b = true;
    }

    // Save current theme
    if (ImGui::Button("Save theme"))
        add_current_theme_to_the_list_of_recorded_themes();
    ImGui::SameLine();
    ImGui::TextUnformatted("as");
    ImGui::SameLine();
    if (ImGui::InputText("##_next_theme_name", &_next_theme_name, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        add_current_theme_to_the_list_of_recorded_themes();
    }

    return b;
}

auto Editor::imgui_theme_selector(bool is_allowed_to_delete_themes) -> bool
{
    bool b = false;

    if (ImGui::BeginCombo("Theme", _current_theme.name().c_str()))
    {
        Theme const* theme_to_delete = nullptr;
        for (auto const& theme : _themes)
        {
            ImGui::PushID(&theme);
            if (ImGui::Selectable(theme.name().c_str()))
            {
                _current_theme = theme;
                apply_current_theme();
                save_current_theme();
                b = true;
            }
            if (is_allowed_to_delete_themes && ImGui::BeginPopupContextItem("##ctx"))
            {
                if (ImGui::Button("Delete theme (This can't be undone!)"))
                    theme_to_delete = &theme;
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
        if (theme_to_delete)
        {
            if (theme_to_delete->name() == _current_theme.name())
            {
                _current_theme.set_name("");
                save_current_theme();
            }
            _themes.erase(std::remove_if(_themes.begin(), _themes.end(), [&](Theme const& theme) { return &theme == theme_to_delete; }), _themes.end());
            save_themes();
        }
        ImGui::EndCombo();
    }

    return b;
}

} // namespace ImStyleEd