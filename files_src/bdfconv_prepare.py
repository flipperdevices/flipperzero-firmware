#!/usr/bin/env python3


def read_lang(name: str) -> set:
    with open('../files/' + name) as file:
        d, _, _, unicode_base, keyboard, *strings = [x.strip() for x in file]

        letters = {int(x, 16) for x in keyboard.split(' ')}
        letters |= {ord(x) for x in ''.join(strings)}

        if unicode_base[0] == '+':
            unicode_base = int(unicode_base, 16)

            with open('../files/' + d, 'rb') as dictionary:
                for line in dictionary:
                    letters |= {x + unicode_base for x in line.strip(b'\x0A')}
        else:
            letters |= {ord(x) for x in open('../files/' + d).read()}

    return letters


letters = set()

with open('../files/menu.txt', 'r') as file:
    for i, line in enumerate(file):
        if i & 1 == 0:
            letters |= {ord(x) for x in line}
        else:
            letters |= read_lang(line.strip())

letters -= {ord("\n")}

fonts = {
    '6x12':  {10003, 10007, },
    '6x13B': {ord('_'), },
}

cmd = 'bdfconv -v -f 1 -m "{0}" {1}.bdf -o {1}.c -n u8g2_font_{1} -d {1}.bdf'

for name, addon in fonts.items():
    letters_str = ','.join([str(x) for x in sorted(letters | addon)])
    print(cmd.format(letters_str, name)+"\n")
