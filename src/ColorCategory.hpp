#pragma once
#include <array>
#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <vector>
#include "ColorElement.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

class ColorCategory {
public:
    explicit ColorCategory(std::vector<ColorElement> elements = {})
        : _elements{std::move(elements)}
    {}

    void add_element(ColorElement const&, bool is_dark_mode);
    void remove_element(ImGuiCol);

    void apply_to(ImGuiStyle&) const;
    void set_from_style(ImGuiStyle const&);

    void update_colors(bool is_dark_mode);

    auto widget(bool is_dark_mode) -> bool;

    auto name() -> auto& { return _name; }
    auto name() const -> auto const& { return _name; }
    auto elements() -> auto& { return _elements; }
    auto elements() const -> auto const& { return _elements; }
    auto color() -> auto& { return _color; }
    auto color() const -> auto const& { return _color; }

private:
    void sort();

private:
    std::string               _name{"Unnamed"};
    std::array<float, 3>      _color{0.f, 0.f, 0.f}; // Not a vec4, the opacity is handled by each element separately
    std::vector<ColorElement> _elements;

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Name", _name),
            cereal::make_nvp("Color", _color),
            cereal::make_nvp("Elements", _elements)
        );
    }
};

auto category_with_all_color_elements() -> ColorCategory;

} // namespace ImStyleEd