#pragma once
#include <cereal/types/vector.hpp>
#include <filesystem>
#include "ColorCategory.hpp"
#include "ColorTheme.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

class CategoryConfig {
public:
    explicit CategoryConfig(std::filesystem::path serialization_file_path, std::vector<ColorCategory> categories = {})
        : _serialization_file_path{std::move(serialization_file_path)}
        , _categories{std::move(categories)}
    {
        load_from_disk();
        update_colors();
        apply_to(ImGui::GetStyle());
    }
    auto widget() -> bool;
    void category_creation_widget();
    void add_category(ColorCategory const& category) { _categories.push_back(category); }
    void apply_to(ImGuiStyle&);

    void set_theme(ColorTheme const&);
    auto get_theme_as(std::string const& name) -> ColorTheme;

private:
    auto categories_table() -> bool;
    void remove_element_from_all_groups(ImGuiCol);
    void save_to_disk();
    void load_from_disk();
    void update_colors();

private:
    std::vector<ColorCategory> _categories;
    bool                       _is_dark_mode{true};

    std::filesystem::path _serialization_file_path{};

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