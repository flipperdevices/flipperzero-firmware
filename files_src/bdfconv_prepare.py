#!/usr/bin/env python3

configs = []
strings = ""


def read_lang(name: str) -> str:
    with open('../files/' + name, 'r') as file:
        d, _, _, unicode_base, keyboard, *strings = list(file)
        letters = ''.join([chr(int(x, 16))
                          for x in keyboard.split(' ')]) + ''.join(strings)

        if unicode_base[0] == '+':
            unicode_base = int(unicode_base, 16)

            with open('../files/' + d.strip(), 'rb') as dictionary:
                for line in dictionary:
                    letters += ''.join([chr(x + unicode_base)
                                       for x in line.strip(b'\x0A')])
        else:
            with open('../files/' + d.strip(), 'r') as dictionary:
                letters += ''.join(dictionary)

    return letters.replace("\n", "")


with open('../files/menu.txt', 'r') as file:
    for i, line in enumerate(file):
        if i & 1 == 0:
            strings += line.strip()
        else:
            strings += read_lang(line.strip())

uniq_codes = [ord(x) for x in set(strings)] + [10003, 10007, ord('_')]
uniq_codes.sort()

cmd = 'bdfconv -v -f 1 -m "{0}" {1}.bdf -o {1}.c -n u8g2_font_{1} -d {1}.bdf'
letters = ','.join([str(x) for x in uniq_codes])

for x in ['6x12', '6x13B']:
    print(cmd.format(letters, x)+"\n")
