#pragma once
#include <string>
#include <vector>
#include "Group.hpp"

namespace ImStyleEd {

struct Category {
    std::string        name{};
    std::vector<Group> groups{};
};

} // namespace ImStyleEd