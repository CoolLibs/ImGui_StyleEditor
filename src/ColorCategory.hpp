#pragma once
#include <array>
#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <vector>
#include "BrightnessGroup.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

class ColorCategory {
public:
    explicit ColorCategory(std::vector<BrightnessGroup> brightness_groups = {})
        : _brightness_groups{std::move(brightness_groups)}
    {}

    void apply_to(ImGuiStyle&) const;
    void set_from_style(ImGuiStyle const&);

    void update_colors(bool is_dark_mode);

    auto widget(bool is_dark_mode) -> bool;

    void add_brightness_group();

    auto name() -> auto& { return _name; }
    auto name() const -> auto const& { return _name; }
    auto brightness_groups() -> auto& { return _brightness_groups; }
    auto brightness_groups() const -> auto const& { return _brightness_groups; }
    auto color() -> auto& { return _color; }
    auto color() const -> auto const& { return _color; }

private:
    void sort();

private:
    std::string                  _name{"Unnamed"};
    std::array<float, 3>         _color{0.f, 0.f, 0.f}; // Not a vec4, the opacity is handled by each element separately
    std::vector<BrightnessGroup> _brightness_groups;

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Name", _name),
            cereal::make_nvp("Color", _color),
            cereal::make_nvp("Brightness groups", _brightness_groups)
        );
    }
};

auto category_with_all_color_elements() -> ColorCategory;

} // namespace ImStyleEd