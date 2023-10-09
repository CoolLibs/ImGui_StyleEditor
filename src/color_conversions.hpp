#pragma once
#include <glm/glm.hpp>

namespace ImStyleEd {

/// All our spaces (except the a and b components of Oklab) are normalized and take values between 0 and 1.

auto Oklab_from_sRGB(glm::vec3 const&) -> glm::vec3;
auto sRGB_from_Oklab(glm::vec3 const&) -> glm::vec3;

} // namespace ImStyleEd