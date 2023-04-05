#include "Editor.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>

namespace ImStyleEd {

void Editor::apply()
{
}

void Editor::save_config()
{
    auto os = std::ofstream{_paths.config_path};
    {
        auto archive = cereal::JSONOutputArchive{os};
        archive(cereal::make_nvp("Config", _config));
    }
}

void Editor::load_config()
{
    auto is = std::ifstream{_paths.config_path};
    if (!is.is_open())
        return;
    try
    {
        auto archive = cereal::JSONInputArchive{is};
        archive(_config);
    }
    catch (...)
    {
    }
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