#pragma once
#include <cereal/cereal.hpp>
#include "imgui/imgui.h"

namespace ImStyleEd {

/// sRGB
struct Color {
    float r{};
    float g{};
    float b{};

    auto as_imvec4() const -> ImVec4 { return {r, g, b, 1.f}; }

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