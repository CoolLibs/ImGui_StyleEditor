#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include "Color.hpp"

namespace ImStyleEd {

/// Function that takes a function and calls it for every category name.
using ForEachCategoryName = std::function<void(
    std::function<void(std::string const&)>
)>;

struct Theme {
    std::string                            name{""};
    bool                                   is_dark_mode{true};
    std::unordered_map<std::string, Color> categories_colors{}; // Maps a category name to a color

    auto color_for_category(std::string const& category_name) const -> Color;
    auto imgui(ForEachCategoryName const&) -> bool;

    void rename_category(std::string const& old_category_name, std::string const& new_category_name);
};

} // namespace ImStyleEd