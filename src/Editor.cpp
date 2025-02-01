#include "Editor.hpp"
#include <algorithm>
#include <cereal/archives/json.hpp>
#include <exception>
#include <filesystem>
#include <fstream>
#include "Element.hpp"
#include "ErrorHandlers.hpp"
#include "GroupID.hpp"
#include "JsonSerializer.hpp"
#include "color_conversions.hpp"
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "wants_dark_theme/wants_dark_theme.hpp"

static void to_json(nlohmann::json& json, ImVec2 const& v)
{
    ImStyleEd::json_set(json, "x", v.x);
    ImStyleEd::json_set(json, "y", v.y);
}
static void from_json(nlohmann::json const& json, ImVec2& v)
{
    ImStyleEd::json_get(json, "x", v.x);
    ImStyleEd::json_get(json, "y", v.y);
}

static void to_json(nlohmann::json& json, ImVec4 const& v)
{
    ImStyleEd::json_set(json, "x", v.x);
    ImStyleEd::json_set(json, "y", v.y);
    ImStyleEd::json_set(json, "z", v.z);
    ImStyleEd::json_set(json, "w", v.w);
}
static void from_json(nlohmann::json const& json, ImVec4& v)
{
    ImStyleEd::json_get(json, "x", v.x);
    ImStyleEd::json_get(json, "y", v.y);
    ImStyleEd::json_get(json, "z", v.z);
    ImStyleEd::json_get(json, "w", v.w);
}

namespace ImStyleEd {

static void to_json(nlohmann::json& json, GroupID const& group_id)
{
    ImStyleEd::json_set(json, "Category", group_id.category_name);
    ImStyleEd::json_set(json, "Group", group_id.group_name);
}
static void from_json(nlohmann::json const& json, GroupID& group_id)
{
    ImStyleEd::json_get(json, "Category", group_id.category_name);
    ImStyleEd::json_get(json, "Group", group_id.group_name);
}

static void to_json(nlohmann::json& json, Element const& element)
{
    ImStyleEd::json_set(json, "Name", element.name);
}
static void from_json(nlohmann::json const& json, Element& element)
{
    ImStyleEd::json_get(json, "Name", element.name);
}

static void to_json(nlohmann::json& json, Group const& group)
{
    ImStyleEd::json_set(json, "Name", group.name);
    ImStyleEd::json_set(json, "Brightness delta", group.brightness_delta);
    ImStyleEd::json_set(json, "Opacity", group.opacity);
}
static void from_json(nlohmann::json const& json, Group& group)
{
    ImStyleEd::json_get(json, "Name", group.name);
    ImStyleEd::json_get(json, "Brightness delta", group.brightness_delta);
    ImStyleEd::json_get(json, "Opacity", group.opacity);
}

static void to_json(nlohmann::json& json, Category const& category)
{
    ImStyleEd::json_set(json, "Name", category.name);
    ImStyleEd::json_set(json, "Behaves differently in light mode", category.behaves_differently_in_light_mode);
    ImStyleEd::json_set(json, "Groups", category.groups);
}
static void from_json(nlohmann::json const& json, Category& category)
{
    ImStyleEd::json_get(json, "Name", category.name);
    ImStyleEd::json_get(json, "Behaves differently in light mode", category.behaves_differently_in_light_mode);
    ImStyleEd::json_get(json, "Groups", category.groups);
}

static void to_json(nlohmann::json& json, Config const& config)
{
    ImStyleEd::json_set(json, "Categories", config.categories());
    for (auto const& element : config.elements())
        ImStyleEd::json_set(json["Element's Group ID"], element.first.name, element.second);
}
static void from_json(nlohmann::json const& json, Config& config)
{
    ImStyleEd::json_get(json, "Categories", config.categories());
    for (auto& element : config.elements())
        ImStyleEd::json_get(json.at("Element's Group ID"), element.first.name, element.second);
}

static void to_json(nlohmann::json& json, Color const& color)
{
    ImStyleEd::json_set(json, "R", color.r);
    ImStyleEd::json_set(json, "G", color.g);
    ImStyleEd::json_set(json, "B", color.b);
}
static void from_json(nlohmann::json const& json, Color& color)
{
    ImStyleEd::json_get(json, "R", color.r);
    ImStyleEd::json_get(json, "G", color.g);
    ImStyleEd::json_get(json, "B", color.b);
}

static void to_json(nlohmann::json& json, Theme const& theme)
{
    ImStyleEd::json_set(json, "Is Dark Mode", theme.is_dark_mode);
    for (auto const& [name, color] : theme.categories_colors)
        ImStyleEd::json_set(json["Categories"], name, color);
}
static void from_json(nlohmann::json const& json, Theme& theme)
{
    ImStyleEd::json_get(json, "Is Dark Mode", theme.is_dark_mode);
    for (auto const& [name, color_json] : json.at("Categories").items())
        from_json(color_json, theme.categories_colors[name]);
}

Editor::Editor(SerializationPaths paths, std::function<void(ImStyleEd::Config&)> const& register_color_elements)
    : _paths{std::move(paths)}
    , _config_serializer{std::make_unique<JsonSerializer<Config>>()}
// , _current_theme_serializer{std::make_unique<JsonSerializer<Theme>>()}
{
    register_color_elements(_config);
    load_config(); // Must be done after registering the elements. Only the registered elements will be loaded from the JSON.
    load_themes();
    if (load_current_theme())
    {
        apply_current_theme(); // Must be done after the config and current theme have been loaded.
    }
    else
    {
        // Try to apply a default theme
        if (!_themes.empty())
        {
            _current_theme = _themes[0].theme;
            save_current_theme();
            apply_current_theme();
        }
    }
}

void Editor::update()
{
    if (!_use_os_theme)
        return;
    _use_os_theme->update(*this);
}

void Editor::OsThemeChecker::update(Editor& editor)
{
    auto const prev_color_mode = _color_mode;
    _color_mode                = Cool::wants_dark_theme().value_or(true) ? Mode::Dark : Mode::Light;
    if (_color_mode == prev_color_mode)
        return;

    editor.apply_theme_if_any(_color_mode == Mode::Light ? "Light" : "Dark");
}

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
            auto const group_color = compute_color(group, category_color, _current_theme.is_dark_mode, category.behaves_differently_in_light_mode);
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
    auto const it = std::find_if(_themes.begin(), _themes.end(), [&](ThemeAndSerializer const& theme) {
        return theme.theme.name == theme_name;
    });
    if (it == _themes.end())
        return nullptr;
    return &it->theme;
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

static void create_folders_if_they_dont_exist(std::filesystem::path const& folder_path)
{
    try
    {
        if (std::filesystem::exists(folder_path))
            return;
        std::filesystem::create_directories(folder_path);
    }
    catch (std::exception const& e)
    {
        error_handlers().on_optional_warning("Failed to create folder \"" + folder_path.string() + "\":\n" + e.what());
    }
}

void Editor::save_config() const
{
    _config_serializer->save(_config, _paths.config_file);
}

void Editor::load_config()
{
    _config_serializer->load(_config, _paths.config_file);
}

void Editor::save_themes() const
{
    for (auto const& theme : _themes)
        save_theme(theme);
}

auto Editor::theme_file_path(Theme const& theme) const -> std::filesystem::path
{
    return _paths.themes_folder / (theme.name + ".json");
}

void Editor::save_theme(ThemeAndSerializer const& theme) const
{
    create_folders_if_they_dont_exist(_paths.themes_folder);
    theme.serializer->save(theme.theme, theme_file_path(theme.theme));
}

auto Editor::find_closest_theme(Theme const& target_theme) const -> Theme const*
{
    for (auto const& theme : _themes)
    {
        // Don't return the exact same theme
        if (&theme.theme == &target_theme)
            continue;
        // Return the first theme that has the same is_dark_mode()
        if (theme.theme.is_dark_mode == target_theme.is_dark_mode)
            return &theme.theme;
    }
    // If no good match, just return the first theme
    for (auto const& theme : _themes)
    {
        // Don't return the exact same theme
        if (&theme.theme == &target_theme)
            continue;
        return &theme.theme;
    }
    return nullptr;
}

void Editor::load_themes()
{
    assert(_themes.empty());
    // Load default themes
    try
    {
        for (auto const& entry : std::filesystem::directory_iterator(_paths.themes_default_folder))
        {
            auto const& path = entry.path();
            if (!std::filesystem::is_regular_file(path) || path.extension() != ".json")
                continue;
            _themes.emplace_back(Theme{path.stem().string()}, std::make_unique<JsonSerializer<Theme>>());
            _themes.back().serializer->load(_themes.back().theme, path);
        }
    }
    catch (std::exception const& e)
    {
        error_handlers().on_optional_warning(std::string{"Failed to load default themes:\n"} + e.what());
    }
    // Load user-defined themes
    create_folders_if_they_dont_exist(_paths.themes_folder); // Prevents directory_iterator from throwing an error that we don't want to display to the user, because it is okay if they don't have a user-defined themes folder.
                                                             // Also this makes sure they have the folder, so it will be easier for them if they want to drag and drop some themes from the internet into this folder, they won't have to create it themselves.
    try
    {
        for (auto const& entry : std::filesystem::directory_iterator(_paths.themes_folder))
        {
            auto const& path = entry.path();
            if (!std::filesystem::is_regular_file(path) || path.extension() != ".json")
                continue;

            auto const theme_name = path.stem().string();

            auto const it = std::find_if(_themes.begin(), _themes.end(), [&](ThemeAndSerializer const& theme) {
                return theme.theme.name == theme_name;
            });
            if (it != _themes.end())
            {
                // Overwrite some of the values of the default theme with the ones in the user-defined theme of the same name
                it->serializer->load(it->theme, path);
            }
            else
            {
                _themes.emplace_back(Theme{theme_name}, std::make_unique<JsonSerializer<Theme>>());
                _themes.back().serializer->load(_themes.back().theme, path);

                // Fill all the missing categories from the new_theme from the ones in the closest_theme
                auto&       new_theme     = _themes.back().theme;
                auto const* closest_theme = find_closest_theme(new_theme);
                if (closest_theme)
                {
                    for (auto const& category : _config.categories())
                    {
                        if (new_theme.categories_colors.find(category.name) == new_theme.categories_colors.end())
                            new_theme.categories_colors[category.name] = closest_theme->color_for_category(category.name);
                    }
                }
            }
        }
    }
    catch (std::exception const& e)
    {
        error_handlers().on_optional_warning(std::string{"Failed to load user-defined themes:\n"} + e.what());
    }

    sort_themes();
}

void Editor::sort_themes()
{
    std::sort(_themes.begin(), _themes.end(), [](ThemeAndSerializer const& a, ThemeAndSerializer const& b) {
        return a.theme.name < b.theme.name;
    });
}

void Editor::save_current_theme() const
{
    // auto os = std::ofstream{_paths.current_theme_file};
    // {
    //     auto archive = cereal::JSONOutputArchive{os};
    //     archive(
    //         cereal::make_nvp("Use OS Theme", _use_os_theme.has_value()),
    //         cereal::make_nvp("Current theme", _current_theme)
    //     );
    // }
}

auto Editor::load_current_theme() -> bool
{
    auto is = std::ifstream{_paths.current_theme_file};
    if (!is.is_open())
        return false;
    try
    {
        bool use_os_theme{};
        {
            // auto archive = cereal::JSONInputArchive{is};
            // archive(
            //     use_os_theme,
            //     _current_theme
            // );
        }
        if (use_os_theme)
            _use_os_theme = OsThemeChecker{};
        else
            _use_os_theme.reset();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void Editor::add_current_theme_to_the_list_of_recorded_themes()
{
    _current_theme.name = _next_theme_name;
    save_current_theme();

    _themes.emplace_back(_current_theme, std::make_unique<JsonSerializer<Theme>>());
    save_theme(_themes.back());

    _next_theme_name = "";

    sort_themes();
}

void Editor::rename_category_in_themes(std::string const& old_category_name, std::string const& new_category_name)
{
    _current_theme.rename_category(old_category_name, new_category_name);
    for (auto& theme : _themes)
        theme.theme.rename_category(old_category_name, new_category_name);
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
        _current_theme.name = "";
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

    if (ImGui::BeginCombo("Theme", _use_os_theme.has_value() ? "Use OS color theme" : _current_theme.name.c_str()))
    {
        {
            bool const is_selected = _use_os_theme.has_value();
            if (ImGui::Selectable("Use OS color theme", is_selected))
            {
                if (!_use_os_theme.has_value())
                    _use_os_theme = OsThemeChecker{};
                b = true;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ThemeAndSerializer const* theme_to_delete = nullptr;
        for (auto const& theme : _themes)
        {
            ImGui::PushID(&theme);
            bool const is_selected = !_use_os_theme.has_value() && theme.theme.name == _current_theme.name;
            if (ImGui::Selectable(theme.theme.name.c_str(), is_selected))
            {
                _use_os_theme.reset();
                _current_theme = theme.theme;
                apply_current_theme();
                save_current_theme();
                b = true;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
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
            if (theme_to_delete->theme.name == _current_theme.name)
            {
                _current_theme.name = "";
                save_current_theme();
            }
            try
            {
                std::filesystem::remove(theme_file_path(theme_to_delete->theme));
            }
            catch (std::exception const& e)
            {
                error_handlers().on_optional_warning("Failed to remove theme file \"" + theme_file_path(theme_to_delete->theme).string() + "\":\n" + e.what());
            }
            _themes.erase(std::remove_if(_themes.begin(), _themes.end(), [&](ThemeAndSerializer const& theme) { return &theme == theme_to_delete; }), _themes.end());
        }
        ImGui::EndCombo();
    }

    return b;
}

} // namespace ImStyleEd