## Architecture

A color (vec3) will be assigned to each Category. Then the Groups inside that Category can tweak its brightness and opacity.

The `ColorCategories` defines groups of colors. Then the `ColorConfig` maps `ColorElement`s to one of the `ColorGroup`s of one of the `Colorcategory`s of the `ColorFamilies`.

Finally, a `ColorTheme` gives colors to each of the `ColorCategory`s.

**`ColorTheme` and `ColorConfig` are the high-level user-facing classes.**

```mermaid
classDiagram
    ColorEditor --|> ColorConfig: Has a
    ColorEditor --|> ColorTheme: Has several
    ColorCategories --|> ColorCategory : Has several
    ColorCategory --|> ColorGroup : Has several
    ColorConfig --> ColorElement : Gives a color group to each
    ColorConfig --> ColorGroupID : Assigns one to each element
    ColorTheme --> ColorCategories : Gives a color to each category
    ColorGroupID --> ColorCategory : Identifies one
    ColorGroupID --> ColorGroup : Identifies one
    class ColorConfig{
        element_to_color_id: map&ltelement->ColorGroupID&gt
        register_element(): To call at the beginning of the app
    }
    class ColorTheme{
        categories_colors : map&ltcategory_name->color_vec3&gt
    }
    class ColorCategories{
        categories: ColorCategory[]
    }
    class ColorCategory{
        name: string
        groups: ColorGroup[]
    }
    class ColorGroup{
        name: string
        brightness_delta: float
        opacity: float
    }
    class ColorElement{
        name: string
        apply_color: functor
    }
    class ColorGroupID{
        category_name: string
        group_name: string
    }
```
