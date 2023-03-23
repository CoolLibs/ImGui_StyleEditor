#include "Editor.hpp"

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
    _config.widget();
    if (ImGui::Button("Save Theme"))
    {
        _themes.add_theme(_config.get_theme());
    }

    return b;
}

} // namespace ImStyleEd