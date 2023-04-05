#include "Config.hpp"
#include <utility>
#include "imgui/imgui.h"

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

auto Config::imgui_categories_table() -> bool
{
    bool b = false;

    static constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame
                                             | ImGuiTableFlags_Resizable
                                             | ImGuiTableFlags_BordersOuter
                                             | ImGuiTableFlags_BordersV
                                             | ImGuiTableFlags_BordersH;

    auto const nb_columns = _categories.size();
    if (nb_columns <= 0)
        return b;

    if (ImGui::BeginTable("color_theme_categories", static_cast<int>(nb_columns), flags))
    {
        for (auto& category : _categories)
        {
            ImGui::TableSetupColumn(category.name.c_str());
        }
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();

        ImGui::EndTable();
    }

    //     ColorCategory const* category_to_remove     = nullptr;
    //     ColorCategory const* category_to_move_left  = nullptr;
    //     ColorCategory const* category_to_move_right = nullptr;
    //     for (size_t column = 0; column < nb_columns; column++)
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

} // namespace ImStyleEd