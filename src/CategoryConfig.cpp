#include "CategoryConfig.hpp"
#include <exe_path/exe_path.h>
#include <cereal/archives/json.hpp>
#include <filesystem>
#include <fstream>
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

namespace ImStyleEd {

void CategoryConfig::widget()
{
    bool b = false;
    b |= ImGui::Checkbox("Is Dark Mode", &_is_dark_mode);
    for (auto& category : _categories)
    {
        b |= category.widget(_is_dark_mode);
    }
    if (b)
    {
        update_colors();
        // apply_to(ImGui::GetStyle());
    }
}

void CategoryConfig::category_creation_widget()
{
    if (ImGui::Button("Add Category"))
        _categories.emplace_back();
    ImGui::SameLine();
    if (ImGui::Button("Apply All"))
    {
        update_colors();
        apply_to(ImGui::GetStyle());
    }

    categories_table();
    // apply_to(ImGui::GetStyle());
    save_to_disk();
}

void CategoryConfig::apply_to(ImGuiStyle& style)
{
    for (auto& category : _categories)
        category.apply_to(style);
}

void CategoryConfig::set_from_style(ImGuiStyle const& style)
{
    for (auto& category : _categories)
        category.set_from_style(style);
}

void CategoryConfig::categories_table()
{
    static constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame
                                             | ImGuiTableFlags_Resizable
                                             | ImGuiTableFlags_BordersOuter
                                             | ImGuiTableFlags_BordersV
                                             | ImGuiTableFlags_BordersH;

    auto const nb_columns = _categories.size();
    if (nb_columns > 0 && ImGui::BeginTable("tex_library", static_cast<int>(nb_columns), flags))
    {
        for (auto& category : _categories)
        {
            ImGui::TableSetupColumn(category.name().c_str());
        }
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        for (size_t column = 0; column < nb_columns; column++)
        {
            ImGui::TableSetColumnIndex(static_cast<int>(column));
            auto& category = _categories[column];
            ImGui::PushID(&category);
            ImGui::InputText("", &category.name());
            ImGui::PopID();
            ImGui::BeginGroup();
            for (auto& group : category.brightness_groups())
            {
                if (group.widget())
                    group.update_color(category.color(), _is_dark_mode);
            }
            ImGui::BeginDisabled();
            ImGui::Button("Drag elements here");
            ImGui::EndDisabled();
            ImGui::EndGroup();
            if (ImGui::BeginDragDropTarget())
            {
                // if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
                // {
                //     IM_ASSERT(payload->DataSize == sizeof(ImGuiCol));
                //     ImGuiCol payload_element = *(const ImGuiCol*)payload->Data;
                //     remove_element_from_all_categories(payload_element);
                //     category.add_element(payload_element, _is_dark_mode);
                // }
                ImGui::EndDragDropTarget();
            }
        }
        ImGui::EndTable();
    }
}

// void CategoryConfig::remove_element_from_all_categories(ColorElement const& element)
// {
//     for (auto& category : _categories)
//         category.remove_element(element.id());
// }

static auto path() -> std::filesystem::path const&
{
    static auto const p = exe_path::dir() / "color_config.json";
    return p;
}

void CategoryConfig::save_to_disk()
{
    std::ofstream os{path()};
    {
        cereal::JSONOutputArchive archive{os};
        archive(cereal::make_nvp("Config", *this));
    }
}

void CategoryConfig::load_from_disk()
{
    std::ifstream is{path()};
    if (!is.is_open())
        return;
    try
    {
        cereal::JSONInputArchive archive{is};
        archive(*this);
    }
    catch (...)
    {
    }
}

void CategoryConfig::update_colors()
{
    for (auto& category : _categories)
        category.update_colors(_is_dark_mode);
}

} // namespace ImStyleEd