#pragma once
#include "ColorCategory.h"

namespace ImStyleEd {

class CategoryConfig {
public:
    void widget();

private:
    void categories_table();

private:
    std::vector<ColorCategory> _categories;
};

} // namespace ImStyleEd