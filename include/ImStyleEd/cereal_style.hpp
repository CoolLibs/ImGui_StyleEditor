#pragma once

#include <imgui/imgui.h>

namespace cereal {

template<class Archive>
void serialize(Archive& archive, ImVec2& v)
{
    archive(
        cereal::make_nvp("x", v.x),
        cereal::make_nvp("y", v.y)
    );
}

template<class Archive>
void serialize(Archive& archive, ImVec4& v)
{
    archive(
        cereal::make_nvp("x", v.x),
        cereal::make_nvp("y", v.y),
        cereal::make_nvp("z", v.z),
        cereal::make_nvp("w", v.w)
    );
}

template<class Archive>
void serialize(Archive& archive, ImGuiStyle& style)
{
    archive(
        cereal::make_nvp("Alpha", style.Alpha),
        cereal::make_nvp("DisabledAlpha", style.DisabledAlpha),
        cereal::make_nvp("WindowPadding", style.WindowPadding),
        cereal::make_nvp("WindowRounding", style.WindowRounding),
        cereal::make_nvp("WindowBorderSize", style.WindowBorderSize),
        cereal::make_nvp("WindowMinSize", style.WindowMinSize),
        cereal::make_nvp("WindowTitleAlign", style.WindowTitleAlign),
        cereal::make_nvp("WindowMenuButtonPosition", style.WindowMenuButtonPosition),
        cereal::make_nvp("ChildRounding", style.ChildRounding),
        cereal::make_nvp("ChildBorderSize", style.ChildBorderSize),
        cereal::make_nvp("PopupRounding", style.PopupRounding),
        cereal::make_nvp("PopupBorderSize", style.PopupBorderSize),
        cereal::make_nvp("FramePadding", style.FramePadding),
        cereal::make_nvp("FrameRounding", style.FrameRounding),
        cereal::make_nvp("FrameBorderSize", style.FrameBorderSize),
        cereal::make_nvp("ItemSpacing", style.ItemSpacing),
        cereal::make_nvp("ItemInnerSpacing", style.ItemInnerSpacing),
        cereal::make_nvp("CellPadding", style.CellPadding),
        cereal::make_nvp("TouchExtraPadding", style.TouchExtraPadding),
        cereal::make_nvp("IndentSpacing", style.IndentSpacing),
        cereal::make_nvp("ColumnsMinSpacing", style.ColumnsMinSpacing),
        cereal::make_nvp("ScrollbarSize", style.ScrollbarSize),
        cereal::make_nvp("ScrollbarRounding", style.ScrollbarRounding),
        cereal::make_nvp("GrabMinSize", style.GrabMinSize),
        cereal::make_nvp("GrabRounding", style.GrabRounding),
        cereal::make_nvp("LogSliderDeadzone", style.LogSliderDeadzone),
        cereal::make_nvp("TabRounding", style.TabRounding),
        cereal::make_nvp("TabBorderSize", style.TabBorderSize),
        cereal::make_nvp("TabMinWidthForCloseButton", style.TabMinWidthForCloseButton),
        cereal::make_nvp("ColorButtonPosition", style.ColorButtonPosition),
        cereal::make_nvp("ButtonTextAlign", style.ButtonTextAlign),
        cereal::make_nvp("SelectableTextAlign", style.SelectableTextAlign),
        cereal::make_nvp("SeparatorTextBorderSize", style.SeparatorTextBorderSize),
        cereal::make_nvp("SeparatorTextAlign", style.SeparatorTextAlign),
        cereal::make_nvp("SeparatorTextPadding", style.SeparatorTextPadding),
        cereal::make_nvp("DisplayWindowPadding", style.DisplayWindowPadding),
        cereal::make_nvp("DisplaySafeAreaPadding", style.DisplaySafeAreaPadding),
        cereal::make_nvp("MouseCursorScale", style.MouseCursorScale),
        cereal::make_nvp("AntiAliasedLines", style.AntiAliasedLines),
        cereal::make_nvp("AntiAliasedLinesUseTex", style.AntiAliasedLinesUseTex),
        cereal::make_nvp("AntiAliasedFill", style.AntiAliasedFill),
        cereal::make_nvp("CurveTessellationTol", style.CurveTessellationTol),
        cereal::make_nvp("CircleTessellationMaxError", style.CircleTessellationMaxError),
        cereal::make_nvp("HoverFlagsForTooltipMouse", style.HoverFlagsForTooltipMouse),
        cereal::make_nvp("HoverFlagsForTooltipNav", style.HoverFlagsForTooltipNav),
        cereal::make_nvp("HoverDelayShort", style.HoverDelayShort),
        cereal::make_nvp("HoverDelayNormal", style.HoverDelayNormal),
        cereal::make_nvp("HoverStationaryDelay", style.HoverStationaryDelay)
    );
}

} // namespace cereal