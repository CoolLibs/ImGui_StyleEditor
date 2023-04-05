#pragma once
#include <string>
#include <unordered_map>
#include "Color.hpp"

namespace ImStyleEd {

class Theme {
public:
private:
    std::unordered_map<std::string, Color> categories_colors{}; // Maps a category name to a color
};

} // namespace ImStyleEd