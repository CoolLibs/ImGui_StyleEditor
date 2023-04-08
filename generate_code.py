
from pipe import *
from dataclasses import dataclass
from typing import List


def imgui_col_enum() -> str:
    import os
    path = os.path.join(os.path.dirname(os.path.abspath(
        __file__)), "build/_deps/quick_imgui-src/lib/imgui/imgui.h")

    with open(path, 'r') as file:
        content = file.read()
    begin = """enum ImGuiCol_
{"""
    begin_idx = content.find(begin)
    end_idx = content.find("};", begin_idx)
    return content[begin_idx + len(begin): end_idx]


@dataclass
class ColorElement:
    id: str
    comment: str | None


def parse_color_elements() -> List[ColorElement]:
    def find_comment(line: str):
        index = line.find('//')
        if index == -1:
            return None
        return line[index + 2:].strip()

    def parse_one_line(line: str):
        return ColorElement(
            id=line.split()[0].strip().removesuffix(','),
            comment=find_comment(line),
        )

    res = list(
        imgui_col_enum().split('\n')
        | where(lambda l: l != "")
        | map(parse_one_line)
        | where(lambda x: x.id != "ImGuiCol_COUNT")
    )

    return res


def register_all_imgui_color_elements():
    elems = parse_color_elements()
    res = ""
    for elem in elems:
        res += f'''
config.register_element({{
    "{elem.id.removeprefix("ImGuiCol_")}",
    []()
    {{
        return ImGui::GetStyle().Colors[{elem.id}];
    }},
    [](ImVec4 const& color)
    {{
        ImGui::GetStyle().Colors[{elem.id}] = color;
    }},
    "{elem.comment or ""}",
}});
'''
    return res


def main():
    from tooling.generate_files import generate
    generate(folder="src/generated", files=[
        register_all_imgui_color_elements,
    ])


if __name__ == '__main__':
    main()
