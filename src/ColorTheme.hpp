#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace ImStyleEd {

class ColorTheme {
public:
private:
    std::unordered_map<std::string, glm::vec3> categories_colors{}; // Maps a category name to a color
};

} // namespace ImStyleEd