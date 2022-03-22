#!/usr/bin/env python3

from flipper.app import App
from flipper.utils.fff import FlipperFormatFile
import json
from os.path import basename, join
import shutil
import zlib

class Main(App):
    def init(self):
        self.subparsers = self.parser.add_subparsers(help="sub-command help")

        # generate
        self.parser_generate = self.subparsers.add_parser(
            "generate", help="Generate update description file"
        )

        self.parser_generate.add_argument("-d", dest="directory", required=True)
        self.parser_generate.add_argument(
            "-t", dest="target", required=True
        )
        self.parser_generate.add_argument(
            "-dfu", dest="dfu", required=True
        )
        self.parser_generate.add_argument(
            "-stage", dest="stage", required=True
        )
        self.parser_generate.add_argument(
            "-radio", dest="radiobin", required=False
        )
        self.parser_generate.add_argument(
            "-radioaddr", dest="radioaddr", required=False
        )

        self.parser_generate.set_defaults(func=self.generate)

    def generate(self):
        meta = {}
        file = FlipperFormatFile()
        file.setHeader("Flipper firmware upgrade configuration", 1)
        file.writeKey("Target", self.args.target[1:]) # dirty 'f' strip
        shutil.copyfile(self.args.stage, join(self.args.directory, basename(self.args.stage)))
        file.writeKey("Loader", basename(self.args.stage))
        file.writeComment("little-endian hex!")
        crc = self.crc(self.args.stage)
        file.writeKey("Loader CRC", list(self.batch(crc,2))[::-1])
        shutil.copyfile(self.args.dfu, join(self.args.directory, basename(self.args.dfu)))
        file.writeKey("Firmware", basename(self.args.dfu))
        file.writeKey("Radio", self.args.radiobin or "")
        file.writeKey("Radio address", self.args.radioaddr or 0)
        file.save("%s/update.cfg" % self.args.directory)
        return 0

    @staticmethod
    def crc(fileName):
        prev = 0
        with open(fileName,"rb") as file:
            for eachLine in file:
                prev = zlib.crc32(eachLine, prev)
        return "%08X" % (prev & 0xFFFFFFFF)

    @staticmethod
    def batch(iterable, n=1):
        l = len(iterable)
        for ndx in range(0, l, n):
            yield iterable[ndx:min(ndx + n, l)]

if __name__ == "__main__":
    Main()()
