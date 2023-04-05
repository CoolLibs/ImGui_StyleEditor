#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <utility>
#include <vector>
#include "Category.hpp"
#include "Element.hpp"
#include "Group.hpp"
#include "GroupID.hpp"

namespace ImStyleEd {

/// An element assigned to a group
using GroupedElement = std::pair<Element, GroupID>;

class Config {
public:
    void register_element(Element const&);
    auto imgui() -> bool;

private:
    auto imgui_categories_table() -> bool;

    /// Returns a vector for each Group of each Category, containing the GroupedElements that belong to this group.
    /// + one last vector containing the elements that don't belong to any group.
    auto elements_per_group() -> std::vector<std::vector<GroupedElement*>>; // Stores non-null pointer. Storing references is annoying because of language details.

private:
    std::vector<Category>       _categories{};
    std::vector<GroupedElement> _element_to_group_id{};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Categories", _categories),
            cereal::make_nvp("Elements", _element_to_group_id)
        );
    }
};

} // namespace ImStyleEd