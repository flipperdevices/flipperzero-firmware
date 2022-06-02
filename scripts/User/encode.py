import logging
import argparse
import subprocess
import io
import os
import sys

w = open("out.bm","wb")

def _convert_image_to_bm(pair: set):
    source_filename, destination_filename = pair
    image = file2image(source_filename)
    image.write(destination_filename)

#output = subprocess.check_output(["convert", file, "xbm:-"])
output = subprocess.check_output(["cat", "file.xbm"])
f = io.StringIO(output.decode().strip())
width = int(f.readline().strip().split(" ")[2])
height = int(f.readline().strip().split(" ")[2])

data = f.read().strip().replace("\n", "").replace(" ", "").split("=")[1][:-1]
data_str = data[1:-1].replace(",", " ").replace("0x", "")

data_bin = bytearray.fromhex(data_str)

data_encoded_str = subprocess.check_output(
    ["heatshrink", "-e", "-w8", "-l4"], input=data_bin
)

assert data_encoded_str

data_enc = bytearray(data_encoded_str)
data_enc = bytearray([len(data_enc) & 0xFF, len(data_enc) >> 8]) + data_enc
if len(data_enc) < len(data_bin) + 1:
    data = b"\x01\x00" + data_enc
else:
    data = b"\x00" + data_bin

w.write(data)

