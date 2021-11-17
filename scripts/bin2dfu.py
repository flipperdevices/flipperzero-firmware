#!/usr/bin/env python3

import os
import struct
from zlib import crc32

from flipper.app import App


class Main(App):
    def init(self):
        self.parser.add_argument("-i", "--input", help=".bin input path", required=True)
        self.parser.add_argument(
            "-o", "--output", help=".dfu output path", required=True
        )
        self.parser.add_argument(
            "-a",
            "--address",
            help="Flash address",
            type=lambda x: int(x, 0),
            required=True,
        )
        self.parser.add_argument(
            "-l", "--label", help="DFU Target label", required=True
        )
        self.parser.add_argument(
            "--vid", help="USB Vendor ID", default=0x0483, type=lambda x: int(x, 0)
        )
        self.parser.add_argument(
            "--pid", help="USB Product ID", default=0xDF11, type=lambda x: int(x, 0)
        )

        self.parser.set_defaults(func=self.convert)

    def convert(self):
        if not os.path.exists(self.args.input):
            self.logger.error(f'"{self.args.input}" does not exist')
            return 1

        with open(self.args.input, mode="rb") as file:
            bin = file.read()

        data = struct.pack(
            "<II",
            self.args.address,  # dwElementAddress
            len(bin)            # dwElementSize
        ) + bin                 # Data

        # Target prefix
        #
        szTargetName = self.args.label.encode("ascii")

        data = struct.pack(
            "<6sBI255sII",
            b"Target",          # szSignature
            0,                  # bAlternateSetting
            1,                  # bTargetNamed
            szTargetName,       # szTargetName
            len(data),          # dwTargetSize
            1                   # dwNbElements
        ) + data

        # Prefix
        #
        data = struct.pack(
            "<5sBIB",
            b"DfuSe",           # szSignature
            0x01,               # bVersion,
            len(data)+11,       # DFUImageSize,
            1                   # bTargets
        ) + data

        # Suffix
        #
        data += struct.pack(
            "<HHHH3sB",
            0xFFFF,         # bcdDevice
            self.args.pid,       # idProduct
            self.args.vid,       # idVendor
            0x011a,         # bdcDFU
            b"UFD",         # ucDfuSignature
            16              # bLength
        )

        dwCRC = ~crc32(data) & 0xFFFFFFFF

        data += struct.pack(
            "<I",
            dwCRC           # dwCRC
        )

        open(self.args.output, "wb").write(data)
        return 0


if __name__ == "__main__":
    Main()()
