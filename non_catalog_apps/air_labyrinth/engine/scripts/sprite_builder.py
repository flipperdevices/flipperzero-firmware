#!/usr/bin/env python3
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

def process_sprites(input_dir, output_dir):
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if file.startswith('.'):
                continue
            
            try:
                input_file_path = os.path.join(root, file)
                rel_path = os.path.relpath(input_file_path, input_dir)
                new_rel_path = os.path.splitext(rel_path)[0] + ".fxbm"
                output_file_path = os.path.join(output_dir, new_rel_path)

                os.makedirs(os.path.dirname(output_file_path), exist_ok=True)

                print(f"Converting '{rel_path}' to '{new_rel_path}'")
                xbm = image2xbm(input_file_path)
                img_data = xbm2fxbm(xbm)
                with open(output_file_path, "wb") as f:
                    f.write(img_data)
            except Exception as e:
                print(f"Cannot convert '{rel_path}': {e}")

def clear_directory(directory):
    for root, dirs, files in os.walk(directory, topdown=False):
        for name in files:
            os.remove(os.path.join(root, name))
        for name in dirs:
            os.rmdir(os.path.join(root, name))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("source", help="Source directory")
    parser.add_argument("target", help="Target directory")
    args = parser.parse_args()

    logging.basicConfig(level=logging.ERROR)
    logger = logging.getLogger(__name__)
    logger.debug(f"Building sprites from {args.source} to {args.target}")

    clear_directory(args.target)
    process_sprites(args.source, args.target)

    return 0

if __name__ == "__main__":
    main()