#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/unordered_map.hpp>
#include <unordered_map>
#include "sRGBColor.hpp"

namespace ImStyleEd {

struct ColorTheme {
    std::string                                name{"Unnamed theme"};
    std::unordered_map<std::string, sRGBColor> colors{};
    bool                                       is_dark_mode{};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Colors", colors),
            cereal::make_nvp("Is Dark Mode", is_dark_mode)
        );
    }
};

} // namespace ImStyleEd