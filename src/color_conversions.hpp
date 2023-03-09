#pragma once
#include <glm/glm.hpp>

namespace ImStyleEd {

/// All our spaces (except the a and b components of CIELAB) are normalized and take values between 0 and 1.

[[nodiscard]] auto CIELAB_from_sRGB(glm::vec3 const&) -> glm::vec3;
[[nodiscard]] auto sRGB_from_CIELAB(glm::vec3 const&) -> glm::vec3;

} // namespace ImStyleEd