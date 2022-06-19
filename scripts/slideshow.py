#!/usr/bin/env python3

import os
import struct
from zlib import crc32

from flipper.app import App
from flipper.assets.icon import file2image


class Main(App):
    MAGIC = 0x72676468

    def init(self):
        self.parser.add_argument("-i", "--input", help="input folder", required=True)
        self.parser.add_argument("-o", "--output", help="output file", required=True)

        self.parser.set_defaults(func=self.pack)

    def _icon2header(self, file):
        image = file2image(file)
        return image.width, image.height, image.data_as_carray()

    def pack(self):
        if not os.path.exists(self.args.input):
            self.logger.error(f'"{self.args.input}" does not exist')
            return 1

        slideshow_data = bytes()
        file_idx = 0
        images = []
        while True:
            self.logger.debug(
                f'working on {os.path.join(self.args.input, f"frame_{file_idx:02}.png")}'
            )
            try:
                images.append(
                    file2image(
                        os.path.join(self.args.input, f"frame_{file_idx:02}.png")
                    )
                )
                self.logger.info(f"Processed #{file_idx}")
                file_idx += 1
            except Exception as e:
                break

        widths = set(img.width for img in images)
        heights = set(img.height for img in images)
        if len(widths) != 1 or len(heights) != 1:
            self.logger.error("All images must have same dimensions!")
            return 2

        data = struct.pack(
            "<IBBB", self.MAGIC, widths.pop(), heights.pop(), len(images)
        )
        for image in images:
            data += struct.pack("<H", len(image.data))
            data += image.data

        with open(self.args.output, mode="wb") as file:
            file.write(data)

        return 0


if __name__ == "__main__":
    Main()()
