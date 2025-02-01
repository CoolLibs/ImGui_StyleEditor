#pragma once
#include <string>

namespace ImStyleEd {

struct Group {
    explicit Group(std::string name = "Unnamed Group")
        : name{std::move(name)}
    {}
    std::string name{};
    float       brightness_delta{0.f}; // Between -1 and 1. 0 means using the same color as the category, > 0 means using a brighter one, and < 0 a darker one.
    float       opacity{1.f};
};

} // namespace ImStyleEd