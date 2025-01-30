#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <functional>
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

/// First arg: old name
/// Second arg: new name
using AfterCategoryRenamed = std::function<void(std::string const&, std::string const&)>;

class Config {
public:
    void register_element(Element const&);
    auto imgui(AfterCategoryRenamed const&) -> bool;

    [[nodiscard]] auto categories() const -> auto const& { return _categories; }
    [[nodiscard]] auto categories() -> auto& { return _categories; }
    [[nodiscard]] auto elements() const -> auto const& { return _element_to_group_id; }
    [[nodiscard]] auto elements() -> auto& { return _element_to_group_id; }

    /// Returns a vector for each Group of each Category, containing the GroupedElements that belong to this group.
    auto elements_per_group() -> std::vector<std::vector<GroupedElement*>>; // Stores non-null pointer. Storing references is annoying because of language details.
    /// Returns a vector containing the elements that don't belong to any group.
    auto unassigned_elements() -> std::vector<GroupedElement*>; // Stores non-null pointer. Storing references is annoying because of language details.

private:
    auto imgui_categories_table(AfterCategoryRenamed const&) -> bool;

    [[nodiscard]] auto make_unique_category_name(std::string const& category_name) const -> std::string;
    [[nodiscard]] auto is_unique_category_name(std::string const& category_name) const -> bool;

    void maybe_remove_category(Category const*);
    void maybe_move_category_left(Category const*);
    void maybe_move_category_right(Category const*);

private:
    std::vector<Category>       _categories{};
    std::vector<GroupedElement> _element_to_group_id{};
};

} // namespace ImStyleEd