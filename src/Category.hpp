#pragma once
#include <string>
#include <vector>
#include "Group.hpp"

namespace ImStyleEd {

struct Category {
    std::string        name{"Unnamed Category"};
    std::vector<Group> groups{{Group{}}};
};

} // namespace ImStyleEd