import lief
from lief import ELF

# dirty address table generator
def main():
    print("hello")
    fliplib = lief.parse("../build/STM32.elf")

    fliplib.header.file_type = ELF.E_TYPE.DYNAMIC

    symdefs = list((sym.name, sym.value) for sym in fliplib.exported_symbols)
    symdefs = sorted(symdefs, key=lambda v: v[0])
    with open("fw_rel.table", "wb") as fwdef:
        for symdef in symdefs:
            fwdef.write(bytes("{:s}: {:d}\n".format(symdef[0], symdef[1]), "ascii"))

    print("goodbye")


if __name__ == '__main__':
    main()