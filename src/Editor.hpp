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
        // _config.try_load_from(paths.config_path); // Must be done after registering the elements. Only the registered elements will be loaded from the JSON.
        //                                           // DEV: The saving will happen automatically in the same file when the config gets destroyed
        //                                           // DEV: this deserializes a map and then for each registerd element assigns a ColorGroupID to it if it is in the deserialized map
        apply();
    }

    // Applies the current Theme to all the registered Elements.
    void apply();

    auto imgui_config_editor() -> bool;
    auto imgui_themes_editor() -> bool;

private:
    void save_config();
    void save_themes();

private:
    std::vector<Theme> _themes{};
    Theme              _current_theme{};
    Config             _config{};
    SerializationPaths _paths{};
};

} // namespace ImStyleEd