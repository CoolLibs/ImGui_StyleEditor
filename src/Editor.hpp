#pragma once
#include <filesystem>
#include <vector>
#include "Config.hpp"
#include "Theme.hpp"

namespace ImStyleEd {

struct SerializationPaths {
    std::filesystem::path current_theme;
    std::filesystem::path themes;
    std::filesystem::path config;
};

class Editor {
public:
    /// Loads the themes, applies the current theme.
    /// RegisterColorElements must be a functor that takes an `ImStyleEd::Config&` and registers all the desired color elements into it.
    template<typename RegisterColorElements>
    Editor(SerializationPaths paths, RegisterColorElements const& register_color_elements)
        : _paths{std::move(paths)}
    {
        register_color_elements(_config);
        load_config(); // Must be done after registering the elements. Only the registered elements will be loaded from the JSON.
        load_themes();
        load_current_theme();
        apply_current_theme(); // Must be done after the config and current theme have been loaded.
    }

    /// Applies the current theme to all the registered elements.
    void apply_current_theme();

    /// Applies the theme with the given name if it exists. Also, sets it as the current theme.
    void apply_theme_if_any(std::string_view theme_name);

    auto imgui_config_editor() -> bool;
    auto imgui_themes_editor() -> bool;
    auto imgui_theme_selector() -> bool;

private:
    void save_config();
    void save_themes();
    void save_current_theme();
    void load_config();
    void load_themes();
    void load_current_theme();

    void add_current_theme_to_the_list_of_recorded_themes();

    void remove_unknown_categories_from_theme(Theme& theme) const;
    void remove_unknown_categories_from_themes();

    void rename_category_in_themes(std::string const& old_category_name, std::string const& new_category_name);

private:
    Theme              _current_theme{};
    std::vector<Theme> _themes{};
    Config             _config{};
    SerializationPaths _paths{};
    std::string        _next_theme_name{};
};

} // namespace ImStyleEd