#pragma once
#include <vector>
#include "ColorTheme.hpp"

namespace ImStyleEd {

class ColorThemesManager {
public:
    void add_theme(ColorTheme const&);
    void set_current_theme(ColorTheme const&);
    auto current_theme() -> ColorTheme const&;
    auto widget_theme_picker() -> bool;

private:
    std::vector<ColorTheme> _themes{};
    ColorTheme              _current_theme{};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Current Theme", _current_theme),
            cereal::make_nvp("Themes", _themes)
        );
    }
};

} // namespace ImStyleEd