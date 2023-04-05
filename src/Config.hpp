#pragma once
#include <string>
#include <utility>
#include <vector>
#include "Category.hpp"
#include "Element.hpp"
#include "Group.hpp"
#include "GroupID.hpp"

namespace ImStyleEd {

class Config {
public:
    void register_element(Element const&);
    auto imgui() -> bool;

private:
    auto imgui_categories_table() -> bool;

private:
    std::vector<Category>                    _categories{};
    std::vector<std::pair<Element, GroupID>> _element_to_group_id{};
};

} // namespace ImStyleEd