#pragma once
#include <filesystem>
#include <vector>
#include "Config.hpp"
#include "Theme.hpp"

namespace ImStyleEd {

struct SerializationPaths {
    std::filesystem::path themes_path;
    std::filesystem::path config_path;
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
        apply();
    }

    // Applies the current Theme to all the registered Elements.
    void apply();

    auto imgui_config_editor() -> bool;
    auto imgui_themes_editor() -> bool;

private:
    void save_config();
    void save_themes();
    void load_config();
    void load_themes();

    void remove_unknown_categories_from_theme(Theme& theme);
    void remove_unknown_categories_from_themes();

private:
    Theme              _current_theme{};
    std::vector<Theme> _themes{};
    Config             _config{};
    SerializationPaths _paths{};
};

} // namespace ImStyleEd