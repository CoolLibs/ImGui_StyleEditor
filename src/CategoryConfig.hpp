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
    void set_from_style(ImGuiStyle const&);

private:
    void categories_table();
    void element_widget(ColorElement&);
    void remove_element_from_all_categories(ColorElement const&);

private:
    std::vector<ColorCategory> _categories;
};

} // namespace ImStyleEd