#pragma once
#include <cereal/types/vector.hpp>
#include <filesystem>
#include <vector>
#include "ColorTheme.hpp"

namespace ImStyleEd {

class ColorThemesManager {
public:
    explicit ColorThemesManager(std::filesystem::path serialization_file_path);

    void add_theme(ColorTheme const&);
    void set_current_theme(ColorTheme const&);
    auto current_theme() -> ColorTheme const&;
    auto widget_theme_picker() -> bool;

    void save_to_disk();
    void load_from_disk();

    auto find(std::string_view theme_name) -> ColorTheme const*;

private:
    std::vector<ColorTheme> _themes{};
    ColorTheme              _current_theme{};

    std::filesystem::path _serialization_file_path{};

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