#include "ColorThemesManager.hpp"
#include <imgui/imgui.h>

namespace ImStyleEd {

void ColorThemesManager::add_theme(ColorTheme const& theme)
{
    _themes.push_back(theme);
}

auto ColorThemesManager::current_theme() -> ColorTheme const&
{
    return _current_theme;
}

auto ColorThemesManager::widget_theme_picker() -> bool
{
    bool b = false;
    if (ImGui::BeginCombo("Theme", _current_theme.name.c_str()))
    {
        for (auto const& theme : _themes)
        {
            ImGui::PushID(&theme);
            if (ImGui::Selectable(theme.name.c_str()))
            {
                _current_theme = theme;
                b              = true;
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
    return b;
}

} // namespace ImStyleEd