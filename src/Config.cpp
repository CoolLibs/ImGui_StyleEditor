#include "Config.hpp"
#include <algorithm>
#include <optional>
#include <string>
#include <utility>
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

namespace ImStyleEd {

void Config::register_element(Element const& element)
{
    _element_to_group_id.emplace_back(element, GroupID{});
}

[[nodiscard]] auto Config::is_unique_category_name(std::string const& category_name) const -> bool
{
    return std::none_of(_categories.begin(), _categories.end(), [&](Category const& category) {
        return category.name == category_name;
    });
}

[[nodiscard]] static auto is_unique_group_name(Category const& category, std::string const& group_name) -> bool
{
    return std::none_of(category.groups.begin(), category.groups.end(), [&](Group const& group) {
        return group.name == group_name;
    });
}

[[nodiscard]] auto Config::make_unique_category_name(std::string const& category_name) const -> std::string
{
    auto name      = category_name;
    bool is_unique = is_unique_category_name(name);
    int  i         = 1;
    while (!is_unique)
    {
        name = category_name + "(" + std::to_string(i) + ")";
        i++;
        is_unique = is_unique_category_name(name);
    }
    return name;
}

[[nodiscard]] static auto make_unique_group_name(Category const& category, std::string const& group_name) -> std::string
{
    auto name      = group_name;
    bool is_unique = is_unique_group_name(category, name);
    int  i         = 1;
    while (!is_unique)
    {
        name = group_name + "(" + std::to_string(i) + ")";
        i++;
        is_unique = is_unique_group_name(category, name);
    }
    return name;
}

auto Config::imgui(AfterCategoryRenamed const& after_category_renamed) -> bool
{
    bool b = false;

    if (ImGui::Button("Add Category"))
    {
        _categories.emplace_back(make_unique_category_name("Unnamed Category"));
        b = true;
    }

    b |= imgui_categories_table(after_category_renamed);

    return b;
}

static auto imgui_add_group_button(Category& category) -> bool
{
    if (ImGui::Button("Add group"))
    {
        category.groups.emplace_back(make_unique_group_name(category, "Unnamed Group"));
        return true;
    }
    return false;
}

static void imgui_color_element(GroupedElement& element)
{
    ImGui::BeginGroup(); // To make sure that displaying the helper tooltip doesn't prevent us from knowing if the color widget has been deactivated after edit.
    auto color = ImVec4{element.first.get_color()};
    if (ImGui::ColorEdit4(
            element.first.name.c_str(),
            reinterpret_cast<float*>(&color), // NOLINT(*reinterpret-cast)
            ImGuiColorEditFlags_None
                | ImGuiColorEditFlags_NoInputs
                | ImGuiColorEditFlags_AlphaPreview
                | ImGuiColorEditFlags_NoDragDrop
        ))
    {
        element.first.set_color(color);
    }
    if (!ImGui::IsItemActive() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        {                                   // Set payload to carry the index of our item (could be anything)
            GroupedElement* ptr = &element; // /!\ Assumes this is safe to store this pointer as it should never change (`_element_to_group_id` doesn't change unless we call `register_element()`, which should only happen once at the creation of the Editor / Config.)
            ImGui::SetDragDropPayload("color_theme_drag_drop", &ptr, sizeof(GroupedElement*));
        }

        // Display preview
        ImGui::TextUnformatted(element.first.name.c_str());

        ImGui::EndDragDropSource();
    }
    // Description
    if (!element.first.description.empty())
    {
        ImGui::SameLine();
        ImGui::TextDisabled("%s", "(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(element.first.description.c_str());
            ImGui::EndTooltip();
        }
    }
    ImGui::EndGroup();
}

static auto imgui_color_group(
    Category const&                     category,
    Group&                              group,
    std::vector<GroupedElement*> const& elements,
    std::string const&                  category_name,
    std::optional<std::string> const&   new_category_name,
    std::function<void()> const&        imgui_after_separator_text
) -> bool
{
    bool b = false;

    ImGui::BeginGroup();
    ImGui::PushID(&group);
    {
        ImGui::SeparatorText(group.name.c_str());
        imgui_after_separator_text();
        {
            auto name = group.name; // Don't do the ImGui widget on group.name directly otherwise make_unique_group_name() will see the new modified name and think it is not unique.
            if (ImGui::InputText("", &name))
            {
                group.name = make_unique_group_name(category, name); // If two groups end up with the same name, their elements will get merged and it is not possible to undo that merge because elements only know the name of their group, and so if two groups end up with the same name we can't distinguish them anymore :(
                // Update GroupIDs to the new group name
                for (auto* element : elements)
                {
                    element->second.group_name = group.name;
                }
            }
        }
        b |= ImGui::SliderFloat("Brightness", &group.brightness_delta, -1.f, 1.f);
        b |= ImGui::SliderFloat("Opacity", &group.opacity, 0.f, 1.f);
        for (auto* element : elements)
        {
            imgui_color_element(*element);
            b |= ImGui::IsItemDeactivatedAfterEdit(); // Re-apply the color theme as soon as we stop playing with this color.

            // Update GroupIDs to the new category name
            if (new_category_name)
                element->second.category_name = *new_category_name;
        }
    }
    ImGui::PopID();
    ImGui::EndGroup();

    if (ImGui::BeginDragDropTarget())
    {
        if (ImGuiPayload const* payload = ImGui::AcceptDragDropPayload("color_theme_drag_drop"))
        {
            IM_ASSERT(payload->DataSize == sizeof(GroupedElement*));
            GroupedElement* element       = *reinterpret_cast<GroupedElement* const*>(payload->Data); // NOLINT(*reinterpret-cast)
            element->second.group_name    = group.name;
            element->second.category_name = category_name;
            b                             = true;
        }
        ImGui::EndDragDropTarget();
    }

    return b;
}

void Config::maybe_remove_category(Category const* category_to_remove)
{
    if (!category_to_remove)
        return;

    _categories.erase(std::remove_if(_categories.begin(), _categories.end(), [&](Category const& category) {
                          return &category == category_to_remove;
                      }),
                      _categories.end());
}

void Config::maybe_move_category_left(Category const* category_to_move_left)
{
    if (!category_to_move_left)
        return;

    auto const it = std::find_if(_categories.begin(), _categories.end(), [&](Category const& category) {
        return &category == category_to_move_left;
    });
    if (it == _categories.end() || it == _categories.begin())
        return;

    std::swap(*it, *std::prev(it));
}

void Config::maybe_move_category_right(Category const* category_to_move_right)
{
    if (!category_to_move_right)
        return;

    auto const it = std::find_if(_categories.begin(), _categories.end(), [&](Category const& category) {
        return &category == category_to_move_right;
    });
    if (it == _categories.end())
        return;

    auto const next = std::next(it);
    if (next == _categories.end())
        return;

    std::swap(*it, *next);
}

static void maybe_remove_group(Group const* group_to_remove, Category& category)
{
    if (!group_to_remove)
        return;

    category.groups.erase(std::remove_if(category.groups.begin(), category.groups.end(), [&](Group const& group) {
                              return &group == group_to_remove;
                          }),
                          category.groups.end());
}

static void maybe_move_group_down(Group const* group_to_move_down, Category& category)
{
    if (!group_to_move_down)
        return;

    auto const it = std::find_if(category.groups.begin(), category.groups.end(), [&](Group const& group) {
        return &group == group_to_move_down;
    });
    if (it == category.groups.end())
        return;

    auto const next = std::next(it);
    if (next == category.groups.end())
        return;

    std::swap(*it, *next);
}

static void maybe_move_group_up(Group const* group_to_move_up, Category& category)
{
    if (!group_to_move_up)
        return;

    auto const it = std::find_if(category.groups.begin(), category.groups.end(), [&](Group const& group) {
        return &group == group_to_move_up;
    });
    if (it == category.groups.end() || it == category.groups.begin())
        return;

    std::swap(*it, *std::prev(it));
}

auto Config::imgui_categories_table(AfterCategoryRenamed const& after_category_renamed) -> bool
{
    bool b = false;

    static constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame
                                             | ImGuiTableFlags_Resizable
                                             | ImGuiTableFlags_BordersOuter
                                             | ImGuiTableFlags_BordersV
                                             | ImGuiTableFlags_BordersH;

    auto const nb_categories = static_cast<int>(_categories.size());

    if (ImGui::BeginTable("color_theme_categories", nb_categories + 1, flags))
    {
        for (auto& category : _categories)
        {
            ImGui::TableSetupColumn(category.name.c_str());
        }
        ImGui::TableSetupColumn("Unassigned");

        ImGui::TableHeadersRow();
        ImGui::TableNextRow();

        auto const elements       = elements_per_group();
        auto       elements_index = 0;

        Category const* category_to_remove     = nullptr;
        Category const* category_to_move_left  = nullptr;
        Category const* category_to_move_right = nullptr;
        for (int column = 0; column < nb_categories; column++)
        {
            ImGui::TableSetColumnIndex(column);
            auto& category = _categories[column];
            ImGui::PushID(&category);
            {
                // Category name
                auto new_category_name = std::optional<std::string>{};
                {
                    auto name = category.name; // Don't do the ImGui widget on category.name directly otherwise make_unique_category_name() will see the new modified name and think it is not unique.
                    if (ImGui::InputText("", &name))
                    {
                        auto const old_name = category.name;
                        category.name       = make_unique_category_name(name); // If two categories end up with the same name, their elements will get merged and it is not possible to undo that merge because elements only know the name of their category, and so if two categories end up with the same name we can't distinguish them anymore :(
                        new_category_name   = category.name;
                        after_category_renamed(old_name, category.name);
                        b = true;
                    }
                }
                // Move category
                if (ImGui::Button("Move Left"))
                {
                    category_to_move_left = &category;
                }
                ImGui::SameLine();
                if (ImGui::Button("Move Right"))
                {
                    category_to_move_right = &category;
                }
                // Category Params
                b |= ImGui::Checkbox("Invert brightness of groups when using a Light theme", &category.behaves_differently_in_light_mode);
                // Groups
                Group const* group_to_remove    = nullptr;
                Group const* group_to_move_up   = nullptr;
                Group const* group_to_move_down = nullptr;
                for (auto& group : category.groups)
                {
                    b |= imgui_color_group(category, group, elements[elements_index], category.name, new_category_name, [&]() {
                        if (ImGui::Button("Move Up"))
                        {
                            group_to_move_up = &group;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Move Down"))
                        {
                            group_to_move_down = &group;
                        }
                    });
                    // Remove Group
                    {
                        bool const cant_remove_group = !elements[elements_index].empty();
                        ImGui::BeginGroup();
                        ImGui::BeginDisabled(cant_remove_group);
                        if (ImGui::Button("Remove group"))
                        {
                            group_to_remove = &group;
                            b               = true;
                        }
                        ImGui::EndDisabled();
                        ImGui::EndGroup();
                        if (cant_remove_group && ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted("Can't remove a group that is not empty. Please first move all the elements to other groups.");
                            ImGui::EndTooltip();
                        }
                    }
                    elements_index++;
                }
                maybe_remove_group(group_to_remove, category);
                maybe_move_group_down(group_to_move_down, category);
                maybe_move_group_up(group_to_move_up, category);
                b |= imgui_add_group_button(category);
                // Remove category
                {
                    bool const cant_remove_category = !category.groups.empty();
                    ImGui::BeginGroup();
                    ImGui::BeginDisabled(cant_remove_category);
                    if (ImGui::Button("Remove Category"))
                    {
                        category_to_remove = &category;
                        b                  = true;
                    }
                    ImGui::EndDisabled();
                    ImGui::EndGroup();
                    if (cant_remove_category && ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted("Can't remove a category that is not empty. Please first move all the elements to other categories and delete all the groups.");
                        ImGui::EndTooltip();
                    }
                }
            }
            ImGui::PopID();
        }
        { // Last column of unassigned elements
            ImGui::TableSetColumnIndex(nb_categories);
            auto const unassigned = unassigned_elements();
            ImGui::BeginGroup();
            for (auto* element : unassigned)
            {
                imgui_color_element(*element);
            }
            ImGui::Dummy(ImGui::GetContentRegionAvail()); // To still have a drag-drop target even when no elements are unassigned.
            ImGui::EndGroup();

            if (ImGui::BeginDragDropTarget())
            {
                if (ImGuiPayload const* payload = ImGui::AcceptDragDropPayload("color_theme_drag_drop"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(GroupedElement*));
                    GroupedElement* element       = *reinterpret_cast<GroupedElement* const*>(payload->Data); // NOLINT(*reinterpret-cast)
                    element->second.group_name    = "";
                    element->second.category_name = "";
                    b                             = true;
                }
                ImGui::EndDragDropTarget();
            }
        }

        ImGui::EndTable();

        maybe_remove_category(category_to_remove);
        maybe_move_category_left(category_to_move_left);
        maybe_move_category_right(category_to_move_right);
    }

    return b;
}

auto Config::elements_per_group() -> std::vector<std::vector<GroupedElement*>>
{
    std::vector<std::vector<GroupedElement*>> res;

    // Traverse each group and find the elements that correspond to it.
    for (auto const& category : _categories)
    {
        for (auto const& group : category.groups)
        {
            res.emplace_back();
            for (auto& grouped_element : _element_to_group_id)
            {
                if (grouped_element.second.group_name == group.name
                    && grouped_element.second.category_name == category.name)
                {
                    res.back().emplace_back(&grouped_element);
                }
            }
        }
    }

    // Sort elements by name
    for (auto& vector : res)
    {
        std::sort(vector.begin(), vector.end(), [](GroupedElement const* a, GroupedElement const* b) {
            return a->first.name < b->first.name;
        });
    }

    return res;
}

auto Config::unassigned_elements() -> std::vector<GroupedElement*>
{
    std::vector<GroupedElement*> res;

    // Traverse each element to see if it is in no group at all.
    for (auto& grouped_element : _element_to_group_id)
    {
        bool found = false;
        for (auto const& category : _categories)
        {
            if (category.name != grouped_element.second.category_name)
                continue;
            for (auto const& group : category.groups)
            {
                if (group.name == grouped_element.second.group_name)
                {
                    found = true;
                    break;
                }
            }
        }
        if (!found)
            res.emplace_back(&grouped_element);
    }

    // Sort elements by name
    std::sort(res.begin(), res.end(), [](GroupedElement const* a, GroupedElement const* b) {
        return a->first.name < b->first.name;
    });

    return res;
}

} // namespace ImStyleEd