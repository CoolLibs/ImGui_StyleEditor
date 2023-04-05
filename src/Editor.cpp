#include "Editor.hpp"

namespace ImStyleEd {

void Editor::apply()
{
}

void Editor::save_config()
{
}

void Editor::save_themes()
{
}

auto Editor::imgui_config_editor() -> bool
{
    if (_config.imgui())
    {
        save_config();
        apply();
        return true;
    }
    return false;
}

auto Editor::imgui_themes_editor() -> bool
{
    return false;
}

} // namespace ImStyleEd