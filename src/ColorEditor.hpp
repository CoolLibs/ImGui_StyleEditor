#pragma once
#include <vector>
#include "ColorConfig.hpp"
#include "ColorTheme.hpp"

namespace ImStyleEd {

class ColorEditor {
public:
private:
    std::vector<ColorTheme> _themes;
    ColorTheme              _current_theme;
    ColorConfig             _config;
};

} // namespace ImStyleEd