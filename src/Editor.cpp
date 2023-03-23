#include "Editor.hpp"
#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

namespace ImStyleEd {

Editor::Editor()
    : _config{CategoryConfig{{
        ImStyleEd::category_with_all_color_elements(),
        ImStyleEd::ColorCategory{},
    }}}
{
}

void Editor::widget_color_config()
{
    _config.category_creation_widget();
}

auto Editor::widget_theme_picker() -> bool
{
    bool b = false;

    if (_themes.widget_theme_picker())
    {
        _config.set_theme(_themes.current_theme());
        _config.apply_to(ImGui::GetStyle());
        b = true;
    }
    if (_config.widget())
    {
        _themes.set_current_theme(_config.get_theme_as(""));
    }
    if (ImGui::Button("Save Theme"))
    {
        auto const theme = _config.get_theme_as(_next_theme_name);
        _themes.add_theme(theme);
        _themes.set_current_theme(theme);
        _next_theme_name = "";
    }
    ImGui::SameLine();
    ImGui::TextUnformatted("as");
    ImGui::SameLine();
    ImGui::InputText("##_next_theme_name", &_next_theme_name);

    return b;
}

} // namespace ImStyleEd