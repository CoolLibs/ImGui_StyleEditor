#pragma once
#include <cereal/types/vector.hpp>
#include "ColorCategory.hpp"

namespace ImStyleEd {

class CategoryConfig {
public:
    explicit CategoryConfig(std::vector<ColorCategory> categories = {})
        : _categories{std::move(categories)}
    {
        load_from_disk();
    }
    void widget();
    void category_creation_widget();
    void add_category(ColorCategory const& category) { _categories.push_back(category); }
    void set_from_style(ImGuiStyle const&);
    void apply_to(ImGuiStyle&);

private:
    void categories_table();
    auto element_widget(ColorElement&) -> bool;
    void remove_element_from_all_categories(ColorElement const&);
    void save_to_disk();
    void load_from_disk();

private:
    std::vector<ColorCategory> _categories;

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Categories", _categories)
        );
    }
};

} // namespace ImStyleEd