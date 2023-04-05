#include "Config.hpp"
#include <utility>

namespace ImStyleEd {

void Config::register_element(Element const& element)
{
    _element_to_group_id.emplace_back(element, GroupID{});
}

} // namespace ImStyleEd