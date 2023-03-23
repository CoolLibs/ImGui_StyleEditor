#include "ColorThemesManager.hpp"
#include <imgui/imgui.h>
#include <cereal/archives/json.hpp>
#include <fstream>

namespace ImStyleEd {

ColorThemesManager::ColorThemesManager(std::filesystem::path serialization_file_path)
    : _serialization_file_path{std::move(serialization_file_path)}
{
    load_from_disk();
}

void ColorThemesManager::add_theme(ColorTheme const& theme)
{
    _themes.push_back(theme);
    save_to_disk();
}

void ColorThemesManager::set_current_theme(ColorTheme const& theme)
{
    _current_theme = theme;
    save_to_disk();
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
                save_to_disk();
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
            save_to_disk();
        }
        ImGui::EndCombo();
    }
    return b;
}

void ColorThemesManager::save_to_disk()
{
    std::ofstream os{_serialization_file_path};
    {
        cereal::JSONOutputArchive archive{os};
        archive(cereal::make_nvp("Themes Manager", *this));
    }
}

void ColorThemesManager::load_from_disk()
{
    std::ifstream is{_serialization_file_path};
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

} // namespace ImStyleEd