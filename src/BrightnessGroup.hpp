#pragma once
#include <array>
#include <cereal/cereal.hpp>
#include <string>
#include "color_id_to_string.hpp"
#include "imgui/imgui.h"
#include "sRGBColor.hpp"

namespace ImStyleEd {

class BrightnessGroup {
public:
    BrightnessGroup(std::vector<ImGuiCol> ids = {}, ImVec4 color = {0.f, 0.f, 0.f, 1.f}, float brightness_level = 0.f)
        : _ids{ids}
        , _color{color}
        , _brightness_level{brightness_level}
    {}

    void apply_to(ImGuiStyle&) const;
    void update_color(sRGBColor const& color, bool is_dark_mode);

    void add_element(ImGuiCol id, sRGBColor const& color, bool is_dark_mode);
    void remove_element(ImGuiCol id);
    void sort();

    auto brightness_level() const -> float { return _brightness_level; }

    auto widget() -> bool;

    auto color() -> auto& { return _color; }
    auto color() const -> auto const& { return _color; }

    auto name() const -> std::string;

    auto is_empty() const -> bool { return _ids.empty(); }

private:
    std::vector<ImGuiCol> _ids{};
    ImVec4                _color{};               // TODO(JF) Color should be computed based on the category, the _brightness_level and the _opacity.
    float                 _brightness_level{0.f}; // 0 means using the same color as the category, > 0 means using a brighter one, and < 0 a darker one;

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("IDs", _ids),
            cereal::make_nvp("Brightness change", _brightness_level),
            cereal::make_nvp("Opacity", _color.w)
        );
    }
};

} // namespace ImStyleEd