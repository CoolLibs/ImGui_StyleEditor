#include "ColorCategory.hpp"
#include <algorithm>
#include "imgui/imgui.h"

namespace ImStyleEd {

void ColorCategory::apply_to(ImGuiStyle& style) const
{
    for (auto const& element : _elements)
        element.apply_to(style);
}

void ColorCategory::set_from_style(ImGuiStyle const& style)
{
    for (auto& element : _elements)
        element.set_from_style(style);
}

void ColorCategory::update_colors()
{
    for (auto& element : _elements)
        element.update_color(_color);
}

void ColorCategory::add_element(ColorElement const& element)
{
    _elements.push_back(element);
    sort();
    update_colors();
    apply_to(ImGui::GetStyle());
}

void ColorCategory::remove_element(ImGuiCol id)
{
    _elements.erase(std::remove_if(_elements.begin(), _elements.end(), [&](ColorElement const& elem) { return elem.id() == id; }), _elements.end());
    sort();
}

void ColorCategory::sort()
{
    std::sort(_elements.begin(), _elements.end(), [](ColorElement const& a, ColorElement const& b) {
        return std::string{a.name()} < std::string{b.name()};
    });
}

auto category_with_all_color_elements() -> ColorCategory
{
    return ColorCategory{std::vector<ColorElement>{
#include "generated/list_all_color_elements.inl"
    }};
}

auto ColorCategory::widget() -> bool
{
    bool b = ImGui::ColorEdit3(name().c_str(), color().data(), ImGuiColorEditFlags_NoInputs);
    if (b)
    {
        for (auto& element : _elements)
            element.update_color(_color);
    }
    return b;
}

} // namespace ImStyleEd