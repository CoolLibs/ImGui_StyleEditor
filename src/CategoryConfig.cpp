#include "CategoryConfig.h"
#include "imgui/imgui.h"

namespace ImStyleEd {

void CategoryConfig::widget()
{
    if (ImGui::Button("Add Category"))
        _categories.emplace_back();

    categories_table();
}

void CategoryConfig::categories_table()
{
    static constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame
                                             | ImGuiTableFlags_Resizable
                                             | ImGuiTableFlags_BordersOuter
                                             | ImGuiTableFlags_BordersV
                                             | ImGuiTableFlags_BordersH
                                             | ImGuiTableFlags_ContextMenuInBody;

    auto const nb_columns = static_cast<int>(_categories.size());
    if (nb_columns > 0 && ImGui::BeginTable("tex_library", nb_columns, flags))
    {
        for (auto& category : _categories)
        {
            ImGui::TableSetupColumn(category.name().c_str());
        }
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        for (int column = 0; column < nb_columns; column++)
        {
            ImGui::TableSetColumnIndex(column);
            auto const& category = _categories[column];
            for (auto const& element : category.elements())
            {
                ImGui::Selectable(element.name());
            }
        }
        ImGui::EndTable();
    }
}

} // namespace ImStyleEd