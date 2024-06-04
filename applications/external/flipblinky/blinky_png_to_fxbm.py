#!/usr/bin/env python3

# This script was modified from
# https://github.com/flipperdevices/flipperzero-game-engine/blob/dev/scripts/sprite_builder.py

# input file "blinky.png"
# output file "blinky.fxbm"

import argparse
import io
import logging
import os
import struct

from PIL import Image, ImageOps

# XBM flipper sprite (.fxbm) is 1-bit depth, width-padded to 8 bits
# file format:
#   uint32 size of the rest of the file in bytes
#   uint32 width in px
#   uint32 height in px
#   uint8[] pixel data, every row is padded to 8 bits (like XBM)


def image2xbm(input_file_path):
    with Image.open(input_file_path) as im:
        with io.BytesIO() as output:
            bw = im.convert("1")
            bw = ImageOps.invert(bw)
            bw.save(output, format="XBM")
            return output.getvalue()


def xbm2fxbm(data):
    # hell as it is, but it works
    f = io.StringIO(data.decode().strip())
    width = int(f.readline().strip().split(" ")[2])
    height = int(f.readline().strip().split(" ")[2])
    data = f.read().strip().replace("\n", "").replace(" ", "").split("=")[1][:-1]
    data_str = data[1:-1].replace(",", " ").replace("0x", "")
    image_bin = bytearray.fromhex(data_str)

    output = struct.pack("<I", len(image_bin) + 8)
    output += struct.pack("<II", width, height)
    output += image_bin
    return output


def main():
    print("Converts a B&W 128x64 (or smaller) blinky.png file to blinky.fxbm")
    xbm = image2xbm("blinky.png")
    img_data = xbm2fxbm(xbm)
    with open("blinky.fxbm", "wb") as f:
        f.write(img_data)

    print(
        "Done!  Please copy blinky.fxbm to your Flipper Zero - SD Card/apps_data/flipboard/blinky.fxbm"
    )
    return 0


if __name__ == "__main__":
    main()
