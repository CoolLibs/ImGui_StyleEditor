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
        // { // Apply style with a little bit of delay in order for it to properly apply everywhere.
        //     static int apply_style_next_frame = -1;
        //     if (apply_style_next_frame == 0)
        //     {
        //         apply();
        //         apply_style_next_frame = -1;
        //     }
        //     if (categories_table())
        //     {
        //         apply_style_next_frame = 2;
        //     }
        //     if (apply_style_next_frame >= 0)
        //     {
        //         apply_style_next_frame--;
        //     }
        // }
        return true;
    }
    return false;
}

auto Editor::imgui_themes_editor() -> bool
{
    return false;
}

} // namespace ImStyleEd