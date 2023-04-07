#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <functional>
#include <string>
#include <unordered_map>
#include "Color.hpp"

namespace ImStyleEd {

/// Function that takes a function and calls it for every category name.
using ForEachCategoryName = std::function<void(
    std::function<void(std::string const&)>
)>;

class Theme {
public:
    auto color_for_category(std::string const& category_name) -> Color;
    auto imgui(ForEachCategoryName const&) -> bool;

    [[nodiscard]] auto is_dark_mode() const { return _is_dark_mode; }

private:
    std::unordered_map<std::string, Color> _categories_colors{}; // Maps a category name to a color
    bool                                   _is_dark_mode{true};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Categories colors", _categories_colors),
            cereal::make_nvp("Is dark mode", _is_dark_mode)
        );
    }
};

} // namespace ImStyleEd