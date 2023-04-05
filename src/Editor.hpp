#pragma once
#include <vector>
#include "Config.hpp"
#include "Theme.hpp"

namespace ImStyleEd {

class Editor {
public:
private:
    std::vector<Theme> _themes;
    Theme              _current_theme;
    Config             _config;
};

} // namespace ImStyleEd