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
    Editor(SerializationPaths const& paths, RegisterColorElements const& register_color_elements)
    {
        register_color_elements(_config);
        // _config.try_load_from(paths.config_path); // Must be done after registering the elements. Only the registered elements will be loaded from the JSON.
        //                                           // DEV: The saving will happen automatically in the same file when the config gets destroyed
        //                                           // DEV: this deserializes a map and then for each registerd element assigns a ColorGroupID to it if it is in the deserialized map
    }

    void imgui_config_editor();
    void imgui_themes_editor();

private:
    std::vector<Theme> _themes;
    Theme              _current_theme;
    Config             _config;
};

} // namespace ImStyleEd