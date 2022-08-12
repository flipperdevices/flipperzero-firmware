#!/usr/bin/env python3

from flipper.app import App
import subprocess


class Main(App):
    def init(self):
        self.parser.add_argument("elfname", action="store")
        self.parser.set_defaults(func=self.process)

    def process(self):
        all_sizes = subprocess.check_output(
            ["arm-none-eabi-size", "-A", self.args.elfname], shell=True
        )
        all_sizes = all_sizes.splitlines()

        sections_to_keep = (".text", ".rodata", ".data", ".bss", ".free_flash")
        for line in all_sizes:
            line = line.decode("utf-8")
            parts = line.split()
            if len(parts) != 3:
                continue
            section, size, _ = parts
            if section not in sections_to_keep:
                continue
            print(f"{section:<11} {size:>8} ({(int(size)/1024):6.2f} K)")

        return 0


if __name__ == "__main__":
    Main()()
