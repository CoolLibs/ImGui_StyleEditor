#include "Editor.hpp"
#include <algorithm>
#include <exception>
#include <filesystem>
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

namespace {
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
} // namespace

static void to_json(nlohmann::json& json, internal::CurrentTheme const& current)
{
    std::visit(
        overloaded{
            [&](internal::OsThemeChecker const&) {
                json["Name"] = nullptr;
            },
            [&](std::string const& theme_name) {
                ImStyleEd::json_set(json, "Name", theme_name);
            },
        },
        current.name_or_os_theme
    );
}
static void from_json(nlohmann::json const& json, internal::CurrentTheme& current)
{
    if (json.at("Name").is_null())
        current.name_or_os_theme = internal::OsThemeChecker{};
    else if (json.at("Name").is_string())
        current.name_or_os_theme = json.at("Name").get<std::string>();
}

auto internal::CurrentTheme::name() const -> std::string
{
    return std::visit(
        overloaded{
            [&](OsThemeChecker const& checker) -> std::string {
                switch (checker.color_mode())
                {
                case OsThemeChecker::Mode::Light:
                    return "Light";
                default:
                    return "Dark";
                }
            },
            [&](std::string const& theme_name) {
                return theme_name;
            },
        },
        name_or_os_theme
    );
}

auto internal::CurrentTheme::update() -> bool
{
    return std::visit(
        overloaded{
            [&](OsThemeChecker& checker) {
                return checker.update();
            },
            [&](std::string&) {
                return false;
            },
        },
        name_or_os_theme
    );
}

Editor::Editor(SerializationPaths paths, std::function<void(ImStyleEd::Config&)> const& register_color_elements)
    : _paths{std::move(paths)}
    , _config_serializer{std::make_unique<JsonSerializer<Config>>()}
    , _current_theme_serializer{std::make_unique<JsonSerializer<internal::CurrentTheme>>()}
{
    register_color_elements(_config);
    load_config(); // Must be done after registering the elements. Only the registered elements will be loaded from the JSON.
    load_themes();
    load_current_theme();
    _current_theme.update(); // Allow it to check if os theme is light or dark, otherwise we would load dark theme by default, and on the next frame realize that os is actually light and switch theme, which could cause slight flicker on startup
    apply_current_theme_or_set_default();
}

void Editor::update()
{
    if (_current_theme.update())
        apply_current_theme_or_set_default();
}

auto internal::OsThemeChecker::update() -> bool
{
    auto const prev_color_mode = _color_mode;
    _color_mode                = Cool::wants_dark_theme().value_or(true) ? Mode::Dark : Mode::Light;
    return _color_mode != prev_color_mode;
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

void Editor::apply_theme(Theme const& theme)
{
    auto const elements_per_group = _config.elements_per_group();
    int        idx                = 0;
    for (auto const& category : _config.categories())
    {
        auto const category_color = theme.color_for_category(category.name);
        for (auto const& group : category.groups)
        {
            auto const group_color = compute_color(group, category_color, theme.is_dark_mode, category.behaves_differently_in_light_mode);
            for (auto const& element : elements_per_group[idx])
                element->first.set_color(group_color);
            idx++;
        }
    }
}

void Editor::apply_current_theme_or_set_default()
{
    if (!apply_theme_if_any(_current_theme.name()))
        set_default_theme();
}

auto Editor::apply_theme_if_any(std::string_view theme_name) -> bool
{
    auto const* theme = try_get_theme(theme_name);
    if (!theme)
        return false;
    apply_theme(*theme);
    return true;
}

auto Editor::set_theme_if_any(std::string_view theme_name) -> bool
{
    if (!apply_theme_if_any(theme_name))
        return false;

    _current_theme.name_or_os_theme = std::string{theme_name};
    save_current_theme();
    return true;
}

void Editor::set_theme(Theme const& theme)
{
    apply_theme(theme);
    _current_theme.name_or_os_theme = theme.name;
    save_current_theme();
}

void Editor::set_default_theme()
{
    if (!set_theme_if_any("Dark")
        && !set_theme_if_any("Light"))
    {
        if (!_themes.empty())
            set_theme(_themes[0].theme);
    }
}

auto Editor::get_default_theme() const -> Theme const*
{
    auto const* theme = try_get_theme("Dark");
    if (theme)
        return theme;
    theme = try_get_theme("Light");
    if (theme)
        return theme;
    if (!_themes.empty())
        return &_themes[0].theme;
    return nullptr;
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
auto Editor::try_get_theme(std::string_view theme_name) -> Theme*
{
    auto const it = std::find_if(_themes.begin(), _themes.end(), [&](ThemeAndSerializer const& theme) {
        return theme.theme.name == theme_name;
    });
    if (it == _themes.end())
        return nullptr;
    return &it->theme;
}

auto Editor::get_color_from_theme_or_default(std::string_view theme_name, std::string_view color_category) const -> Color
{
    auto const* theme = try_get_theme(theme_name);
    if (!theme)
        return Color{1.f, 0.f, 1.f};
    return theme->color_for_category(std::string{color_category});
}

auto Editor::get_color(std::string_view color_category) const -> Color
{
    return get_color_from_theme_or_default(_current_theme.name(), color_category);
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
    _current_theme_serializer->save(_current_theme, _paths.current_theme_file);
}

void Editor::load_current_theme()
{
    _current_theme_serializer->load(_current_theme, _paths.current_theme_file);
}

void Editor::save_theme_and_add_it_to_the_list_of_themes(Theme& theme)
{
    _current_theme.name_or_os_theme = theme.name;
    save_current_theme();

    auto const it = std::find_if(_themes.begin(), _themes.end(), [&](ThemeAndSerializer const& th) {
        return th.theme.name == theme.name;
    });
    if (it != _themes.end())
    {
        // Override existing theme
        it->theme = theme;
        save_theme(*it);
    }
    else
    {
        // Create a new theme
        _themes.emplace_back(theme, std::make_unique<JsonSerializer<Theme>>());
        save_theme(_themes.back());
        sort_themes();
    }
}

void Editor::rename_category_in_themes(std::string const& old_category_name, std::string const& new_category_name)
{
    for (auto& theme : _themes)
        theme.theme.rename_category(old_category_name, new_category_name);
}

auto Editor::imgui_config_editor() -> bool
{
    auto const after_category_renamed = [&](std::string const& old_category_name, std::string const& new_category_name) {
        rename_category_in_themes(old_category_name, new_category_name);
        save_themes(); // It is important that this is done after category has been renamed, otherwise it will get removed when we try to remove the unknown categories.
    };
    if (_config.imgui(after_category_renamed))
    {
        apply_current_theme_or_set_default();
        save_config();
        return true;
    }
    return false;
}

static auto file_name_error(std::string const& name) -> std::string
{
    if (name.empty())
        return "Name cannot be empty";
    if (name.length() > 200)
        return "Name is too long";
    if (name == "Temporary")
        return "\"Temporary\" is a special name for a theme that will be overriden any time you make a change to a theme";

    for (char const invalid_char : {'.', '<', '>', ':', '\"', '/', '\\', '|', '?', '*', '\0'})
    {
        if (name.find(invalid_char) != std::string::npos)
            return std::string{"Name cannot contain a "} + invalid_char;
    }

    auto upper_case_name = name;
    std::transform(upper_case_name.begin(), upper_case_name.end(), upper_case_name.begin(), [](char c) {
        return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); // We need those static_casts to avoid undefined behaviour, cf. https://en.cppreference.com/w/cpp/string/byte/toupper
    });
    for (const char* invalid_name : {
             "CON", "PRN", "AUX", "NUL",
             "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
             "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
         })
    {
        if (upper_case_name == invalid_name)
            return '\"' + std::string{name} + '\"' + " is a reserved name";
    }

    return "";
}

auto Editor::imgui_themes_editor() -> bool
{
    bool b = false;

    // Theme selector
    b |= imgui_theme_selector(true /*is_allowed_to_delete_themes*/);

    // Edit current theme
    auto tmp_theme = Theme{};
    if (auto const* theme = try_get_theme(_current_theme.name()))
        tmp_theme = *theme;
    else if (auto const* theme = get_default_theme())
        tmp_theme = *theme;

    if (tmp_theme.imgui([&](std::function<void(std::string const&)> const& callback) {
            for (auto const& category : _config.categories())
                callback(category.name);
        }))
    {
        tmp_theme.name                  = "Temporary";
        _current_theme.name_or_os_theme = "Temporary";
        apply_theme(tmp_theme);
        save_current_theme();
        save_theme_and_add_it_to_the_list_of_themes(tmp_theme);
        b = true;
    }

    // Save current theme
    std::string const err = file_name_error(_next_theme_name);
    ImGui::BeginGroup();
    ImGui::BeginDisabled(!err.empty());
    if (ImGui::Button("Save theme"))
    {
        tmp_theme.name = _next_theme_name;
        save_theme_and_add_it_to_the_list_of_themes(tmp_theme);
        _next_theme_name = "";
    }
    ImGui::EndDisabled();
    ImGui::EndGroup();
    if (!err.empty())
        ImGui::SetItemTooltip("%s", err.c_str());
    ImGui::SameLine();
    ImGui::TextUnformatted("as");
    ImGui::SameLine();
    if (ImGui::InputText("##_next_theme_name", &_next_theme_name, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        if (err.empty())
        {
            tmp_theme.name = _next_theme_name;
            save_theme_and_add_it_to_the_list_of_themes(tmp_theme);
            _next_theme_name = "";
        }
    }

    return b;
}

auto Editor::imgui_theme_selector(bool is_allowed_to_delete_themes) -> bool
{
    bool b = false;

    bool const is_using_os_theme = std::get_if<internal::OsThemeChecker>(&_current_theme.name_or_os_theme);

    if (ImGui::BeginCombo("Theme", is_using_os_theme ? "Use OS color theme" : _current_theme.name().c_str()))
    {
        {
            bool const is_selected = is_using_os_theme;
            if (ImGui::Selectable("Use OS color theme", is_selected))
            {
                if (!is_using_os_theme)
                {
                    _current_theme.name_or_os_theme = internal::OsThemeChecker{};
                    save_current_theme();
                    // No need to apply the theme, it will be applied on the next update() next frame
                    b = true;
                }
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ThemeAndSerializer const* theme_to_delete = nullptr;
        for (auto const& theme : _themes)
        {
            ImGui::PushID(&theme);
            bool const is_selected = !is_using_os_theme && theme.theme.name == _current_theme.name();
            if (ImGui::Selectable(theme.theme.name.c_str(), is_selected))
            {
                _current_theme.name_or_os_theme = theme.theme.name;
                apply_current_theme_or_set_default();
                save_current_theme();
                b = true;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
            if (is_allowed_to_delete_themes && ImGui::BeginPopupContextItem("##ctx"))
            {
                if (ImGui::Button("Delete theme (This can't be undone!)"))
                {
                    theme_to_delete = &theme;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
        if (theme_to_delete)
        {
            try
            {
                std::filesystem::remove(theme_file_path(theme_to_delete->theme));
                if (!is_using_os_theme && theme_to_delete->theme.name == _current_theme.name())
                    set_default_theme();
                _themes.erase(std::remove_if(_themes.begin(), _themes.end(), [&](ThemeAndSerializer const& theme) { return &theme == theme_to_delete; }), _themes.end());
            }
            catch (std::exception const& e)
            {
                error_handlers().on_optional_warning("Failed to remove theme file \"" + theme_file_path(theme_to_delete->theme).string() + "\":\n" + e.what());
            }
        }
        ImGui::EndCombo();
    }

    return b;
}

} // namespace ImStyleEd