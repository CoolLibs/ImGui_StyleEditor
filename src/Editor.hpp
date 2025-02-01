#pragma once
#include <filesystem>
#include <variant>
#include <vector>
#include "Config.hpp"
#include "ISerializer.hpp"
#include "Theme.hpp"

namespace ImStyleEd {

namespace internal {
class OsThemeChecker {
public:
    /// Returns true iff the color theme has changed
    auto update() -> bool;

    enum class Mode : uint8_t {
        Unknown,
        Dark,
        Light,
    };

    auto color_mode() const -> Mode { return _color_mode; }

private:
    Mode _color_mode{Mode::Unknown};
};

struct CurrentTheme {
    std::variant<OsThemeChecker, std::string> name_or_os_theme{OsThemeChecker{}};

    auto name() const -> std::string;
    /// Returns true iff the color theme has changed
    auto update() -> bool;
};
} // namespace internal

struct SerializationPaths {
    std::filesystem::path current_theme_file;
    std::filesystem::path themes_folder;
    std::filesystem::path themes_default_folder;
    std::filesystem::path config_file;
};

class Editor {
public:
    /// Loads the themes, applies the current theme.
    /// `register_color_elements` must be a function that takes an `ImStyleEd::Config&` and registers all the desired color elements into it
    Editor(SerializationPaths paths, std::function<void(ImStyleEd::Config&)> const& register_color_elements);

    /// Must be called once every frame
    void update();

    /// Applies the theme with the given name if it exists. Also, sets it as the current theme.
    auto set_theme_if_any(std::string_view theme_name) -> bool;
    void set_theme(Theme const& theme);
    void set_default_theme();
    auto try_get_theme(std::string_view theme_name) const -> Theme const*;
    auto get_color_from_theme_or_default(std::string_view theme_name, std::string_view color_category) const -> Color;
    auto get_color(std::string_view color_category) const -> Color;

    auto imgui_config_editor() -> bool;
    auto imgui_themes_editor() -> bool;
    auto imgui_theme_selector(bool is_allowed_to_delete_themes = false) -> bool;

private:
    struct ThemeAndSerializer {
        Theme                               theme;
        std::unique_ptr<ISerializer<Theme>> serializer;

        ThemeAndSerializer(Theme theme, std::unique_ptr<ISerializer<Theme>> serializer)
            : theme{std::move(theme)}
            , serializer{std::move(serializer)}
        {}
    };

private:
    auto try_get_theme(std::string_view theme_name) -> Theme*;
    auto find_closest_theme(Theme const& target_theme) const -> Theme const*;
    auto theme_file_path(std::string const& theme_name) const -> std::filesystem::path;
    auto get_default_theme() const -> Theme const*;

    void save_config() const;
    void save_theme(ThemeAndSerializer const&) const;
    void save_themes() const;
    void save_current_theme() const;
    void load_config();
    void load_themes();
    void sort_themes();
    void load_current_theme();
    void delete_theme(std::string const& theme_name);

    auto apply_theme_if_any(std::string_view theme_name) -> bool;
    void apply_current_theme_or_set_default();
    void apply_theme(Theme const&);

    void save_theme_and_add_it_to_the_list_of_themes(Theme& theme);

    void rename_category_in_themes(std::string const& old_category_name, std::string const& new_category_name);

private:
    std::vector<ThemeAndSerializer> _themes{};
    Config                          _config{};
    SerializationPaths              _paths{};
    std::string                     _next_theme_name{};
    internal::CurrentTheme          _current_theme{};

    std::unique_ptr<ISerializer<Config>>                 _config_serializer;
    std::unique_ptr<ISerializer<internal::CurrentTheme>> _current_theme_serializer;
};

} // namespace ImStyleEd