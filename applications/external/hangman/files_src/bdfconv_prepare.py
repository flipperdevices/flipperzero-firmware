#!/usr/bin/env python3


import os

DIR = os.path.dirname(__file__) + "/../files/"


def convert_to_ranges(nums: list) -> str:
    ranges = []
    start = None

    for i, num in enumerate(nums):
        if i == 0 or num != (prev := nums[i - 1]) + 1:
            if start is not None:
                if prev != start:
                    ranges.append("{}-{}".format(start, prev))
                else:
                    ranges.append(str(start))
            start = nums[i]

    if start is not None:
        if (last := nums[-1]) != start:
            ranges.append("{}-{}".format(start, last))
        else:
            ranges.append(str(start))
    return ",".join(ranges)


def read_lang(name: str) -> set:
    with open(DIR + name) as file:
        d, _, _, unicode_base, keyboard, *strings = [x.strip() for x in file]

        letters = {int(x, 16) for x in keyboard.split(" ")}
        letters |= {ord(x) for x in "".join(strings)}

        if unicode_base[0] == "+":
            unicode_base = int(unicode_base, 16)

            with open(DIR + d, "rb") as dictionary:
                for line in dictionary:
                    letters |= {x + unicode_base for x in line.strip(b"\x0A")}
        else:
            letters |= {ord(x) for x in open(DIR + d).read()}

    return letters


letters = set()

with open(DIR + "menu.txt", "r") as file:
    for i, line in enumerate(file):
        if i & 1 == 0:
            letters |= {ord(x) for x in line}
        else:
            letters |= read_lang(line.strip())

letters -= {ord("\n")}

fonts = {
    "6x12": (
        "6x12",
        {
            10003,
            10007,
        },
    ),
    "6x13B": (
        "6x13B-patched",
        {
            ord("_"),
        },
    ),
}

print(
    r"""
(
    declare -A L=( [Y]=04ae [uni018F]=04d8 [afii10147]=04e8);
    awk "!/ENDFONT/ {if (/^CHARS /) print \$1 FS \$2+${#L[@]}+3; else print}" 6x13B.bdf
    for key in "${!L[@]}"; do
        awk "/CHAR $key\$/,/END/{print}" 6x13B.bdf|
        sed "s/^ENC.*/ENCODING $((16#${L[$key]}))/;s/^START.*/STARTCHAR uni${L[$key]}/"
    done;
    echo -e "STARTCHAR uni04ba\nENCODING 1210\nSWIDTH 426 0\nDWIDTH 6 0\nBBX 6 13 0 -2
BITMAP\n00\n00\nC0\nC0\nC0\nC0\nC0\nFC\nCC\nCC\nCC\n00\n00\nENDCHAR
STARTCHAR uni04a2\nENCODING 1186\nSWIDTH 426 0\nDWIDTH 6 0\nBBX 6 13 0 -2
BITMAP\n00\n00\nD8\nD8\nD8\nD8\nF8\nD8\nD8\nD8\nDC\n04\n00\nENDCHAR
STARTCHAR uni0496\nENCODING 1174\nSWIDTH 426 0\nDWIDTH 6 0\nBBX 6 13 0 -2
BITMAP\n00\n00\nA8\nA8\nA8\nF8\n20\nF8\nA8\nA8\nAC\n04\n00\nENDCHAR
ENDFONT"
) > 6x13B-patched.bdf
"""
)

cmd = '../bdfconv/bdfconv -v -f 1 -m "{0}" {2}.bdf -o {1}.c -n u8g2_font_{1}'

for name, (fontname, addon) in fonts.items():
    letters_str = convert_to_ranges(sorted(letters | addon))
    print(cmd.format(letters_str, name, fontname) + "\n")

print("sed 's/U8G2_FONT_SECTION.*/=/' 6x12.c 6x13B.c")
