## Architecture

A color (vec3) will be assigned to each `Category`. Then the `Group`s inside that `Category` can tweak its brightness and opacity.

The `Categories` defines groups of colors. Then the `Config` maps `Element`s to one of the `Group`s of one of the `Category`s of the `Families`.

Finally, a `Theme` gives colors to each of the `Category`s.

**`Editor` is the high-level user-facing class that joins everything together.**

```mermaid
classDiagram
    Editor --|> Config: Has a
    Editor --|> Theme: Has several
    Theme ..> Config : Gives a color to each category
    Category --|> Group : Has several
    Config --|> Category : Has several
    Config ..> GroupID : Assigns one to each element
    Config ..> Element : Gives a group to each
    GroupID ..> Category : Identifies one
    GroupID ..> Group : Identifies one
    class Config{
        element_to_color_id: map&ltelement->GroupID&gt
        register_element(): To call at the beginning of the app
    }
    class Theme{
        categories_colors : map&ltcategory_name->color_vec3&gt
    }
    class Category{
        name: string
        groups: Group[]
    }
    class Group{
        name: string
        brightness_delta: float
        opacity: float
    }
    class Element{
        name: string
        apply_color: functor
    }
    class GroupID{
        category_name: string
        group_name: string
    }
```
