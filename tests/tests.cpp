#include "ImStyleEd/ImStyleEd.hpp"
#include "exe_path/exe_path.h"
#include "imgui.h"
#include "quick_imgui/quick_imgui.hpp"

auto main(int argc, char* argv[]) -> int
{
    const bool should_run_imgui_tests = argc < 2 || strcmp(argv[1], "-nogpu") != 0;
    if (!should_run_imgui_tests)
        return 0;

    quick_imgui::loop("ImStyleEd", [&]() { // Open a window and run all the ImGui-related code
        static auto editor = ImStyleEd::Editor{
            ImStyleEd::SerializationPaths{
                .current_theme_file    = exe_path::dir() / "imstyleed_current_theme.json",
                .themes_folder         = exe_path::dir() / "imstyleed_themes",
                .themes_default_folder = exe_path::dir() / "imstyleed_themes",
                .config_file           = exe_path::dir() / "imstyleed_config.json",
            },
            [](ImStyleEd::Config& config) {
                ImStyleEd::register_all_imgui_color_elements(config);
            }
        };
        ImGui::Begin("Configuration Editor");
        {
            editor.imgui_config_editor();
        }
        ImGui::End();
        ImGui::Begin("Themes Editor");
        {
            editor.imgui_themes_editor();
        }
        ImGui::End();
        ImGui::Begin("Theme Selector");
        {
            editor.imgui_theme_selector();
        }
        ImGui::End();
        ImGui::ShowDemoWindow();
    });
}