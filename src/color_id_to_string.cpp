#include "color_id_to_string.hpp"

namespace ImStyleEd {

auto color_id_to_string(ImGuiCol color_id) -> const char*
{
    switch (color_id)
    {
#include "generated/color_id_to_string.inl"
    default:
    {
        assert(false);
        return "ERROR in color_id_to_string";
    }
    }
}

} // namespace ImStyleEd