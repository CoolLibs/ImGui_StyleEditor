#pragma once
#include "ColorCategory.hpp"

namespace ImStyleEd {

class CategoryConfig {
public:
    explicit CategoryConfig(std::vector<ColorCategory> categories = {})
        : _categories{std::move(categories)}
    {}
    void widget();
    void add_category(ColorCategory const& category) { _categories.push_back(category); }

private:
    void categories_table();

private:
    std::vector<ColorCategory> _categories;
};

} // namespace ImStyleEd