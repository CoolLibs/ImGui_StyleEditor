#pragma once
#include "imgui/imgui.h"

namespace ImStyleEd {

/// sRGB
struct Color {
    float r{};
    float g{};
    float b{};

    auto as_imvec4() const -> ImVec4 { return {r, g, b, 1.f}; }
};

} // namespace ImStyleEd