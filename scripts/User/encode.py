import heatshrink2
import argparse
import io


parser = argparse.ArgumentParser(
    description="Turn .xbm files into cooked .bm files for flipper FS"
)

parser.add_argument("infile", metavar="i", help="Input file")
parser.add_argument("outfile", metavar="o", help="File to write to")

args = vars(parser.parse_args())

with open(args["infile"], "rb") as f:
    output = f.read()
f = io.StringIO(output.decode().strip())

print("Image Dimensions:")
width = int(f.readline().strip().split(" ")[2])
print("W: ", width)
height = int(f.readline().strip().split(" ")[2])
print("H: ", height)


data = f.read().strip().replace("\n", "").replace(" ", "").split("=")[1][:-1]
data_str = data[1:-1].replace(",", " ").replace("0x", "")
data_bin = bytearray.fromhex(data_str)

data_encoded_str = heatshrink2.compress(data_bin, window_sz2=8, lookahead_sz2=4)
data_enc = bytearray(data_encoded_str)
data_enc = bytearray([len(data_enc) & 0xFF, len(data_enc) >> 8]) + data_enc

if len(data_enc) + 2 < len(data_bin) + 1:
    data = b"\x01\x00" + data_enc
else:
    data = b"\x00" + data_bin

with open(args["outfile"], "wb") as f:
    f.write(data)
