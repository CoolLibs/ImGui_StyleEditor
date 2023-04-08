#pragma once
#include <cereal/cereal.hpp>

namespace ImStyleEd {

/// sRGB
struct Color {
    float r{};
    float g{};
    float b{};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("R", r),
            cereal::make_nvp("G", g),
            cereal::make_nvp("B", b)
        );
    }
};

} // namespace ImStyleEd