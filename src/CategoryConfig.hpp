#pragma once
#include <cereal/types/vector.hpp>
#include "ColorCategory.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

class CategoryConfig {
public:
    explicit CategoryConfig(std::vector<ColorCategory> categories = {})
        : _categories{std::move(categories)}
    {
        load_from_disk();
        update_colors();
        apply_to(ImGui::GetStyle());
    }
    void widget();
    void category_creation_widget();
    void add_category(ColorCategory const& category) { _categories.push_back(category); }
    void set_from_style(ImGuiStyle const&);
    void apply_to(ImGuiStyle&);

private:
    auto categories_table() -> bool;
    void remove_element_from_all_groups(ImGuiCol);
    void save_to_disk();
    void load_from_disk();
    void update_colors();

private:
    std::vector<ColorCategory> _categories;
    bool                       _is_dark_mode{true};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Categories", _categories),
            cereal::make_nvp("Is Dark Mode", _is_dark_mode)
        );
    }
};

} // namespace ImStyleEd