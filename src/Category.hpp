#pragma once
#include <string>
#include <vector>
#include "Group.hpp"

namespace ImStyleEd {

struct Category {
    explicit Category(std::string name = "")
        : name{std::move(name)}
    {}
    std::string        name{};
    bool               behaves_differently_in_light_mode{true};
    std::vector<Group> groups{{Group{}}};
};

} // namespace ImStyleEd