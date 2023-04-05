#include "Config.hpp"
#include <algorithm>
#include <optional>
#include <utility>
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

namespace ImStyleEd {

void Config::register_element(Element const& element)
{
    _element_to_group_id.emplace_back(element, GroupID{});
}

auto Config::imgui() -> bool
{
    bool b = false;

    if (ImGui::Button("Add Category"))
    {
        _categories.emplace_back();
        b = true;
    }

    b |= imgui_categories_table();

    return b;
}

static auto imgui_add_group_button(Category& category) -> bool
{
    if (ImGui::Button("Add group"))
    {
        category.groups.emplace_back();
        return true;
    }
    return false;
}

static void imgui_color_element(GroupedElement& element)
{
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
}

static auto imgui_color_group(
    Group&                              group,
    std::vector<GroupedElement*> const& elements,
    std::string const&                  category_name,
    std::optional<std::string> const&   new_category_name
) -> bool
{
    bool b = false;

    ImGui::BeginGroup();
    ImGui::PushID(&group);
    {
        ImGui::SeparatorText(group.name.c_str());
        if (ImGui::InputText("", &group.name))
        {
            group.name = make_unique_group_name(group.name); // If two groups end up with the same name, their elements will get merged and it is not possible to undo that merge because elements only know the name of their group, and so if two groups end up with the same name we can't distinguish them anymore :(
            // Update GroupIDs to the new group name
            for (auto* element : elements)
            {
                element->second.group_name = group.name;
            }
        }
        b |= ImGui::SliderFloat("Brightness", &group.brightness_delta, -1.f, 1.f);
        b |= ImGui::SliderFloat("Opacity", &group.opacity, 0.f, 1.f);
        for (auto* element : elements)
        {
            imgui_color_element(*element);
            // b |= ImGui::IsItemActive(); // TODO(JF) Shouldn't this be ItemDeactivatedAfterEdit if we want to re-apply the theme color as soon as we stop playing with this color.

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

auto Config::imgui_categories_table() -> bool
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

        for (int column = 0; column < nb_categories; column++)
        {
            ImGui::TableSetColumnIndex(column);
            auto& category = _categories[column];
            ImGui::PushID(&category);
            {
                auto new_category_name = std::optional<std::string>{};
                if (ImGui::InputText("", &category.name))
                {
                    category.name     = make_unique_category_name(category.name); // If two categories end up with the same name, their elements will get merged and it is not possible to undo that merge because elements only know the name of their category, and so if two categories end up with the same name we can't distinguish them anymore :(
                    new_category_name = category.name;
                }
                for (auto& group : category.groups)
                {
                    b |= imgui_color_group(group, elements[elements_index], category.name, new_category_name);
                    elements_index++;
                }
                b |= imgui_add_group_button(category);
            }
            ImGui::PopID();
        }
        { // Last column of unassigned elements
            ImGui::TableSetColumnIndex(nb_categories);
            {
                for (auto* element : elements[elements_index])
                {
                    imgui_color_element(*element);
                }
            }
        }

        ImGui::EndTable();
    }

    //     ColorCategory const* category_to_remove     = nullptr;
    //     ColorCategory const* category_to_move_left  = nullptr;
    //     ColorCategory const* category_to_move_right = nullptr;
    //     for (size_t column = 0; column < nb_categories; column++)
    //     {
    //         ImGui::TableSetColumnIndex(static_cast<int>(column));
    //         auto& category = _categories[column];
    //         ImGui::PushID(&category);
    //         ImGui::InputText("", &category.name());
    //         int                    i                  = 0;
    //         BrightnessGroup const* group_to_remove    = nullptr;
    //         BrightnessGroup const* group_to_move_up   = nullptr;
    //         BrightnessGroup const* group_to_move_down = nullptr;
    //         if (ImGui::Button("Move Left"))
    //         {
    //             category_to_move_left = &category;
    //         }
    //         ImGui::SameLine();
    //         if (ImGui::Button("Move Right"))
    //         {
    //             category_to_move_right = &category;
    //         }
    //         if (ImGui::Checkbox("Invert brightness for groups in light mode", &category.behaves_differently_in_light_mode()))
    //         {
    //             category.update_colors(_is_dark_mode);
    //             b = true;
    //         }
    //         for (auto& group : category.brightness_groups())
    //         {
    //             ImGui::BeginGroup();
    //             ImGui::PushID(&group);
    //             ImGui::SeparatorText(("Group " + std::to_string(++i)).c_str());
    //             if (ImGui::Button("Move Up"))
    //             {
    //                 group_to_move_up = &group;
    //             }
    //             ImGui::SameLine();
    //             if (ImGui::Button("Move Down"))
    //             {
    //                 group_to_move_down = &group;
    //             }
    //             if (group.widget())
    //             {
    //                 group.update_color(category.color(), _is_dark_mode, category.behaves_differently_in_light_mode());
    //                 b = true;
    //             }
    //             { // "Remove Group" button
    //                 const bool cant_remove_group = !group.is_empty();

    //                 ImGui::BeginGroup();
    //                 ImGui::BeginDisabled(cant_remove_group);
    //                 if (ImGui::Button("Remove group"))
    //                 {
    //                     group_to_remove = &group;
    //                 }
    //                 ImGui::EndDisabled();
    //                 ImGui::EndGroup();
    //                 if (cant_remove_group && ImGui::IsItemHovered())
    //                 {
    //                     ImGui::BeginTooltip();
    //                     ImGui::TextUnformatted("Can't remove a group that is not empty. Please first move all the elements to other groups.");
    //                     ImGui::EndTooltip();
    //                 }
    //             }
    //             ImGui::PopID();
    //             ImGui::EndGroup();

    //             if (ImGui::BeginDragDropTarget())
    //             {
    //                 if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
    //                 {
    //                     IM_ASSERT(payload->DataSize == sizeof(ImGuiCol));
    //                     ImGuiCol payload_element = *(const ImGuiCol*)payload->Data;
    //                     remove_element_from_all_groups(payload_element);
    //                     group.add_element(payload_element, category.color(), _is_dark_mode, category.behaves_differently_in_light_mode());
    //                     b = true;
    //                 }
    //                 ImGui::EndDragDropTarget();
    //             }
    //         }
    //         if (group_to_remove)
    //         {
    //             category.brightness_groups().erase(std::remove_if(category.brightness_groups().begin(), category.brightness_groups().end(), [&](BrightnessGroup const& group) {
    //                                                    return &group == group_to_remove;
    //                                                }),
    //                                                category.brightness_groups().end());
    //         }
    //         if (group_to_move_down)
    //         {
    //             auto const it = std::find_if(category.brightness_groups().begin(), category.brightness_groups().end(), [&](BrightnessGroup const& group) {
    //                 return &group == group_to_move_down;
    //             });
    //             if (it != category.brightness_groups().end())
    //             {
    //                 auto const next = std::next(it);
    //                 if (next != category.brightness_groups().end())
    //                 {
    //                     std::swap(*it, *next);
    //                 }
    //             }
    //         }
    //         if (group_to_move_up)
    //         {
    //             auto const it = std::find_if(category.brightness_groups().begin(), category.brightness_groups().end(), [&](BrightnessGroup const& group) {
    //                 return &group == group_to_move_up;
    //             });
    //             if (it != category.brightness_groups().end() && it != category.brightness_groups().begin())
    //             {
    //                 std::swap(*it, *std::prev(it));
    //             }
    //         }
    //         if (ImGui::Button("Add group"))
    //         {
    //             category.add_brightness_group();
    //         }
    //         const bool cant_remove_category = !category.is_empty();
    //         ImGui::BeginGroup();
    //         ImGui::BeginDisabled(cant_remove_category);
    //         if (ImGui::Button("Remove Category"))
    //         {
    //             category_to_remove = &category;
    //         }
    //         ImGui::EndDisabled();
    //         ImGui::EndGroup();
    //         if (cant_remove_category && ImGui::IsItemHovered())
    //         {
    //             ImGui::BeginTooltip();
    //             ImGui::TextUnformatted("Can't remove a category that is not empty. Please first move all the elements to other categories.");
    //             ImGui::EndTooltip();
    //         }
    //         ImGui::PopID();
    //     }
    //     ImGui::EndTable();
    //     if (category_to_remove)
    //     {
    //         _categories.erase(std::remove_if(_categories.begin(), _categories.end(), [&](ColorCategory const& category) {
    //                               return &category == category_to_remove;
    //                           }),
    //                           _categories.end());
    //     }
    //     if (category_to_move_right)
    //     {
    //         auto const it = std::find_if(_categories.begin(), _categories.end(), [&](ColorCategory const& category) {
    //             return &category == category_to_move_right;
    //         });
    //         if (it != _categories.end())
    //         {
    //             auto const next = std::next(it);
    //             if (next != _categories.end())
    //             {
    //                 std::swap(*it, *next);
    //             }
    //         }
    //     }
    //     if (category_to_move_left)
    //     {
    //         auto const it = std::find_if(_categories.begin(), _categories.end(), [&](ColorCategory const& category) {
    //             return &category == category_to_move_left;
    //         });
    //         if (it != _categories.end() && it != _categories.begin())
    //         {
    //             std::swap(*it, *std::prev(it));
    //         }
    //     }

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

    // Traverse each element to see if it is in no group at all.
    res.emplace_back();
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
            res.back().emplace_back(&grouped_element);
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

} // namespace ImStyleEd