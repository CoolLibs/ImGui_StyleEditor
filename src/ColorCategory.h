#pragma once
#include <string>
#include <vector>
#include "ColorElement.h"
#include "imgui/imgui.h"

namespace ImStyleEd {

class ColorCategory {
public:
    void apply(ImGuiStyle&);

    auto name() -> auto& { return _name; }
    auto name() const -> auto const& { return _name; }
    auto elements() -> auto& { return _elements; }
    auto elements() const -> auto const& { return _elements; }

private:
    std::string               _name{"Give me a name by clicking HERE"};
    ImVec4                    _color{};
    std::vector<ColorElement> _elements;
};

} // namespace ImStyleEd