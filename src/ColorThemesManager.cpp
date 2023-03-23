#include "ColorThemesManager.hpp"
#include <imgui/imgui.h>

namespace ImStyleEd {

void ColorThemesManager::add_theme(ColorTheme const& theme)
{
    _themes.push_back(theme);
}

void ColorThemesManager::set_current_theme(ColorTheme const& theme)
{
    _current_theme = theme;
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
        ColorTheme const* theme_to_delete = nullptr;
        for (auto const& theme : _themes)
        {
            ImGui::PushID(&theme);
            if (ImGui::Selectable(theme.name.c_str()))
            {
                _current_theme = theme;
                b              = true;
            }
            if (ImGui::BeginPopupContextItem("##ctx"))
            {
                if (ImGui::Button("Delete theme (This can't be undone!)"))
                    theme_to_delete = &theme;
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
        if (theme_to_delete)
        {
            if (theme_to_delete->name == _current_theme.name)
            {
                _current_theme.name = "";
            }
            _themes.erase(std::remove_if(_themes.begin(), _themes.end(), [&](ColorTheme const& theme) { return &theme == theme_to_delete; }), _themes.end());
        }
        ImGui::EndCombo();
    }
    return b;
}

} // namespace ImStyleEd