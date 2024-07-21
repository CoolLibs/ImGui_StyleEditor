#pragma once
#include <imgui/imgui.h>

namespace ser20 {

template<class Archive>
void serialize(Archive& archive, ImVec2& v)
{
    archive(
        ser20::make_nvp("x", v.x),
        ser20::make_nvp("y", v.y)
    );
}

template<class Archive>
void serialize(Archive& archive, ImVec4& v)
{
    archive(
        ser20::make_nvp("x", v.x),
        ser20::make_nvp("y", v.y),
        ser20::make_nvp("z", v.z),
        ser20::make_nvp("w", v.w)
    );
}

template<class Archive>
void serialize(Archive& archive, ImGuiStyle& style)
{
    archive(
        ser20::make_nvp("Alpha", style.Alpha),
        ser20::make_nvp("DisabledAlpha", style.DisabledAlpha),
        ser20::make_nvp("WindowPadding", style.WindowPadding),
        ser20::make_nvp("WindowRounding", style.WindowRounding),
        ser20::make_nvp("WindowBorderSize", style.WindowBorderSize),
        ser20::make_nvp("WindowMinSize", style.WindowMinSize),
        ser20::make_nvp("WindowTitleAlign", style.WindowTitleAlign),
        ser20::make_nvp("WindowMenuButtonPosition", style.WindowMenuButtonPosition),
        ser20::make_nvp("ChildRounding", style.ChildRounding),
        ser20::make_nvp("ChildBorderSize", style.ChildBorderSize),
        ser20::make_nvp("PopupRounding", style.PopupRounding),
        ser20::make_nvp("PopupBorderSize", style.PopupBorderSize),
        ser20::make_nvp("FramePadding", style.FramePadding),
        ser20::make_nvp("FrameRounding", style.FrameRounding),
        ser20::make_nvp("FrameBorderSize", style.FrameBorderSize),
        ser20::make_nvp("ItemSpacing", style.ItemSpacing),
        ser20::make_nvp("ItemInnerSpacing", style.ItemInnerSpacing),
        ser20::make_nvp("CellPadding", style.CellPadding),
        ser20::make_nvp("TouchExtraPadding", style.TouchExtraPadding),
        ser20::make_nvp("IndentSpacing", style.IndentSpacing),
        ser20::make_nvp("ColumnsMinSpacing", style.ColumnsMinSpacing),
        ser20::make_nvp("ScrollbarSize", style.ScrollbarSize),
        ser20::make_nvp("ScrollbarRounding", style.ScrollbarRounding),
        ser20::make_nvp("GrabMinSize", style.GrabMinSize),
        ser20::make_nvp("GrabRounding", style.GrabRounding),
        ser20::make_nvp("LogSliderDeadzone", style.LogSliderDeadzone),
        ser20::make_nvp("TabRounding", style.TabRounding),
        ser20::make_nvp("TabBorderSize", style.TabBorderSize),
        ser20::make_nvp("TabMinWidthForCloseButton", style.TabMinWidthForCloseButton),
        ser20::make_nvp("ColorButtonPosition", style.ColorButtonPosition),
        ser20::make_nvp("ButtonTextAlign", style.ButtonTextAlign),
        ser20::make_nvp("SelectableTextAlign", style.SelectableTextAlign),
        ser20::make_nvp("SeparatorTextBorderSize", style.SeparatorTextBorderSize),
        ser20::make_nvp("SeparatorTextAlign", style.SeparatorTextAlign),
        ser20::make_nvp("SeparatorTextPadding", style.SeparatorTextPadding),
        ser20::make_nvp("DisplayWindowPadding", style.DisplayWindowPadding),
        ser20::make_nvp("DisplaySafeAreaPadding", style.DisplaySafeAreaPadding),
        ser20::make_nvp("MouseCursorScale", style.MouseCursorScale),
        ser20::make_nvp("AntiAliasedLines", style.AntiAliasedLines),
        ser20::make_nvp("AntiAliasedLinesUseTex", style.AntiAliasedLinesUseTex),
        ser20::make_nvp("AntiAliasedFill", style.AntiAliasedFill),
        ser20::make_nvp("CurveTessellationTol", style.CurveTessellationTol),
        ser20::make_nvp("CircleTessellationMaxError", style.CircleTessellationMaxError),
        ser20::make_nvp("HoverFlagsForTooltipMouse", style.HoverFlagsForTooltipMouse),
        ser20::make_nvp("HoverFlagsForTooltipNav", style.HoverFlagsForTooltipNav),
        ser20::make_nvp("HoverDelayShort", style.HoverDelayShort),
        ser20::make_nvp("HoverDelayNormal", style.HoverDelayNormal),
        ser20::make_nvp("HoverStationaryDelay", style.HoverStationaryDelay)
    );
}

} // namespace ser20