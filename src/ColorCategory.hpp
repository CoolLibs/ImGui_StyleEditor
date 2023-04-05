#pragma once
#include <string>
#include <vector>
#include "ColorGroup.hpp"

namespace ImStyleEd {

struct ColorCategory {
    std::string             name{};
    std::vector<ColorGroup> groups{};
};

} // namespace ImStyleEd