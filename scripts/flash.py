#!/usr/bin/env python3

import logging
import argparse
import sys
import os

from flipper.app import App
from flipper.cube import CubeProgrammer


class Main(App):
    def init(self):
        self.subparsers = self.parser.add_subparsers(help="sub-command help")
        self.parser_wipe = self.subparsers.add_parser("wipe", help="Wipe MCU Flash")
        self.parser_wipe.set_defaults(func=self.wipe)
        self.parser_core1 = self.subparsers.add_parser(
            "core1", help="Flash Core1 Firmware"
        )
        self.parser_core1.set_defaults(func=self.core1)
        self.parser_core2fus = self.subparsers.add_parser(
            "core2fus", help="Flash Core2 Firmware Update Service"
        )
        self.parser_core2fus.set_defaults(func=self.core2fus)
        self.parser_core2radio = self.subparsers.add_parser(
            "core2radio", help="Flash Core2 Radio stack"
        )
        self.parser_core2radio.set_defaults(func=self.core2radio)

    def before(self):
        pass

    def wipe(self):
        self.logger.info(f"Wiping flash")
        return 0

    def core1(self):
        return 0

    def core2fus(self):
        return 0

    def core2radio(self):
        return 0


if __name__ == "__main__":
    Main()()
