#include "ColorConfig.hpp"
#include <utility>

namespace ImStyleEd {

void ColorConfig::register_element(ColorElement const& element)
{
    _element_to_group_id.emplace_back(element, ColorGroupID{});
}

} // namespace ImStyleEd