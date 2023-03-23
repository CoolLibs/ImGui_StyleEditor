#pragma once
#include "CategoryConfig.hpp"
#include "ColorThemesManager.hpp"

namespace ImStyleEd {

/// Main class that groups everything together.
class Editor {
public:
    Editor();

    // Simple UI to select your theme or customize it.
    auto widget_theme_picker() -> bool;
    // Advanced UI to control the nitty–gritty details of how a theme is applied to each and every ImGui element.
    void widget_color_config();

private:
    CategoryConfig     _config{};
    ColorThemesManager _themes{};
};

} // namespace ImStyleEd