#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include "Color.hpp"

namespace ImStyleEd {

/// Function that takes a function and calls it for every category name.
using ForEachCategoryName = std::function<void(
    std::function<void(std::string const&)>
)>;

class Theme {
public:
    explicit Theme(std::string name = "")
        : _name{std::move(name)}
    {}
    auto color_for_category(std::string const& category_name) const -> Color;
    auto imgui(ForEachCategoryName const&) -> bool;

    [[nodiscard]] auto is_dark_mode() const { return _is_dark_mode; }
    [[nodiscard]] auto name() const { return _name; }
    void               set_name(std::string name) { _name = std::move(name); }

    void rename_category(std::string const& old_category_name, std::string const& new_category_name);
    void set_colors_for_categories(std::unordered_map<std::string, Color> _new_colors) { _categories_colors = std::move(_new_colors); }

private:
    std::string                            _name{""};
    bool                                   _is_dark_mode{true};
    std::unordered_map<std::string, Color> _categories_colors{}; // Maps a category name to a color

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Name", _name),
            cereal::make_nvp("Is dark mode", _is_dark_mode),
            cereal::make_nvp("Categories colors", _categories_colors)
        );
    }
};

} // namespace ImStyleEd