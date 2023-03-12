
from pipe import *
from dataclasses import dataclass
from typing import List


def imgui_col_enum() -> str:
    import os
    path = os.path.join(os.path.dirname(os.path.abspath(
        __file__)), "build/_deps/quick_imgui-src/lib/imgui/imgui.h")

    res = ""
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
    for item in res:
        print(item)

    return res


def color_id_to_string():
    elems = parse_color_elements()
    res = ""
    for elem in elems:
        res += f'''
case {elem.id}:
return "{elem.id.removeprefix('ImGuiCol_')}";
'''
    return res


def list_all_color_elements():
    elems = parse_color_elements()
    res = ""
    for elem in elems:
        res += f'''
    {elem.id},
'''
    return res


def main():
    from tooling.generate_files import generate
    generate(folder="src/generated", files=[
        color_id_to_string,
        list_all_color_elements,
    ])


if __name__ == '__main__':
    main()
