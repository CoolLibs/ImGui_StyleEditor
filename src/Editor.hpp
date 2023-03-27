#pragma once
#include "CategoryConfig.hpp"
#include "ColorThemesManager.hpp"

namespace ImStyleEd {

struct SerializationPaths {
    std::filesystem::path themes_path;
    std::filesystem::path config_path;
};

/// Main class that groups everything together.
class Editor {
public:
    explicit Editor(SerializationPaths const&);

    // Simple UI to select your theme or customize it.
    auto widget_theme_picker() -> bool;
    // Advanced UI to control the nitty–gritty details of how a theme is applied to each and every ImGui element.
    void widget_color_config();

    // Applies the theme with the given name if it exists.
    void apply_if_any(std::string_view theme_name);

private:
    void save_current_theme();

private:
    CategoryConfig     _config;
    ColorThemesManager _themes;
    std::string        _next_theme_name{};
};

} // namespace ImStyleEd