#pragma once
#include <filesystem>
#include <optional>
#include <vector>
#include "Config.hpp"
#include "ISerializer.hpp"
#include "Theme.hpp"

namespace ImStyleEd {

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

    /// Applies the current theme to all the registered elements.
    void apply_current_theme();

    /// Applies the theme with the given name if it exists. Also, sets it as the current theme.
    void apply_theme_if_any(std::string_view theme_name);
    auto try_get_theme(std::string_view theme_name) const -> Theme const*;
    auto get_color_from_theme_if_any(std::string_view theme_name, std::string_view color_category) const -> Color;
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
    auto find_closest_theme(Theme const& target_theme) const -> Theme const*;
    auto theme_file_path(Theme const& theme) const -> std::filesystem::path;

    void save_config() const;
    void save_theme(ThemeAndSerializer const&) const;
    void save_themes() const;
    void save_current_theme() const;
    void load_config();
    void load_themes();
    void sort_themes();
    /// Returns true iff the theme has successfully been loaded
    auto load_current_theme() -> bool;

    void add_current_theme_to_the_list_of_recorded_themes();

    void rename_category_in_themes(std::string const& old_category_name, std::string const& new_category_name);

private:
    class OsThemeChecker {
    public:
        void update(Editor&);

    private:
        enum class Mode : uint8_t {
            Unknown,
            Dark,
            Light,
        };

    private:
        Mode _color_mode{Mode::Unknown};
    };

private:
    Theme                           _current_theme{};
    std::vector<ThemeAndSerializer> _themes{};
    Config                          _config{};
    SerializationPaths              _paths{};
    std::string                     _next_theme_name{};
    std::optional<OsThemeChecker>   _use_os_theme{OsThemeChecker{}};

    std::unique_ptr<ISerializer<Config>> _config_serializer;
    // std::unique_ptr<ISerializer<Config>> _current_theme_serializer;
};

} // namespace ImStyleEd