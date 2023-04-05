#pragma once
#include "ColorConfig.hpp"
#include "ColorGroupID.hpp"
#include "ColorTheme.hpp"
#include "imgui/imgui.h"

namespace ImStyleEd {

auto get_color(ColorGroupID const&, ColorConfig const&, ColorTheme const&) -> ImVec4

}