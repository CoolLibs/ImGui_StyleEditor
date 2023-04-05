#pragma once
#include <string>
#include <unordered_map>
#include "ColorElement.hpp"
#include "ColorGroup.hpp"
#include "ColorGroupID.hpp"

namespace ImStyleEd {

class ColorCategory {
public:
    void register_element(ColorElement const&);

private:
    std::unordered_map<std::string, ColorGroupID> element_to_group_id{};
};

} // namespace ImStyleEd