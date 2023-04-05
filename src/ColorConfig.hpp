#pragma once
#include <string>
#include <utility>
#include <vector>
#include "ColorElement.hpp"
#include "ColorGroup.hpp"
#include "ColorGroupID.hpp"

namespace ImStyleEd {

class ColorConfig {
public:
    void register_element(ColorElement const&);

private:
    std::vector<std::pair<ColorElement, ColorGroupID>> _element_to_group_id{};
};

} // namespace ImStyleEd