#pragma once
#include <string>
#include <utility>
#include <vector>
#include "Element.hpp"
#include "Group.hpp"
#include "GroupID.hpp"

namespace ImStyleEd {

class Config {
public:
    void register_element(Element const&);

private:
    std::vector<std::pair<Element, GroupID>> _element_to_group_id{};
};

} // namespace ImStyleEd