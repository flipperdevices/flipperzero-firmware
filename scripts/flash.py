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
        # Wipe
        self.parser_wipe = self.subparsers.add_parser("wipe", help="Wipe MCU Flash")
        self.parser_wipe.set_defaults(func=self.wipe)
        # Core 1 boot
        self.parser_core1boot = self.subparsers.add_parser(
            "core1boot", help="Flash Core1 Bootloader"
        )
        self._addArgsSWD(self.parser_core1boot)
        self.parser_core1boot.add_argument("bootloader", type=str, help="Bootloader binary")
        self.parser_core1boot.set_defaults(func=self.core1boot)
        # Core 1 firmware
        self.parser_core1firmware = self.subparsers.add_parser(
            "core1firmware", help="Flash Core1 Firmware"
        )
        self._addArgsSWD(self.parser_core1firmware)
        self.parser_core1firmware.add_argument("firmware", type=str, help="Firmware binary")
        self.parser_core1firmware.set_defaults(func=self.core1firmware)
        # Core 1 all
        self.parser_core1 = self.subparsers.add_parser(
            "core1", help="Flash Core1 Boot and Firmware"
        )
        self._addArgsSWD(self.parser_core1)
        self.parser_core1.add_argument("bootloader", type=str, help="Bootloader binary")
        self.parser_core1.add_argument("firmware", type=str, help="Firmware binary")
        self.parser_core1.set_defaults(func=self.core1)
        # Core 2 fus
        self.parser_core2fus = self.subparsers.add_parser(
            "core2fus", help="Flash Core2 Firmware Update Service"
        )
        self._addArgsSWD(self.parser_core2fus)
        self.parser_core2fus.set_defaults(func=self.core2fus)
        # Core 2 radio stack 
        self.parser_core2radio = self.subparsers.add_parser(
            "core2radio", help="Flash Core2 Radio stack"
        )
        self._addArgsSWD(self.parser_core2radio)
        self.parser_core2radio.set_defaults(func=self.core2radio)

    def _addArgsSWD(self, parser):
        parser.add_argument(
            "--port", type=str, help="Port to connect: swd or usb1", default="swd"
        )

    def wipe(self):
        self.logger.info(f"Wiping flash")
        cp = CubeProgrammer("swd")
        self.logger.info(f"Setting RDP to 0xBB")
        cp.setOptionBytes({"RDP":("0xBB", "rw")})
        self.logger.info(f"Verifying RDP")
        r = cp.checkOptionBytes({"RDP":("0xBB", "rw")})
        assert(r == True)
        self.logger.info(f"Result: {r}")
        self.logger.info(f"Setting RDP to 0xAA")
        cp.setOptionBytes({"RDP":("0xAA", "rw")})
        self.logger.info(f"Verifying RDP")
        r = cp.checkOptionBytes({"RDP":("0xAA", "rw")})
        assert(r == True)
        self.logger.info(f"Result: {r}")
        self.logger.info(f"Complete")
        return 0

    def core1boot(self):
        self.logger.info(f"Flashing bootloader")
        cp = CubeProgrammer(self.args.port)
        cp.flashBin("0x08000000", self.args.bootloader)
        self.logger.info(f"Complete")
        cp.resetTarget()
        return 0

    def core1firmware(self):
        self.logger.info(f"Flashing firmware")
        cp = CubeProgrammer(self.args.port)
        cp.flashBin("0x08008000", self.args.firmware)
        self.logger.info(f"Complete")
        cp.resetTarget()
        return 0

    def core1(self):
        self.logger.info(f"Flashing bootloader")
        cp = CubeProgrammer(self.args.port)
        cp.flashBin("0x08000000", self.args.bootloader)
        self.logger.info(f"Flashing firmware")
        cp.flashBin("0x08008000", self.args.firmware)
        self.logger.info(f"Complete")
        cp.resetTarget()
        return 0

    def core2fus(self):
        return 0

    def core2radio(self):
        return 0


if __name__ == "__main__":
    Main()()
