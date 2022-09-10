#!/usr/bin/env python3
import argparse
import sys
import os

# The code of this function is based on Flipper file plotter by ShotokanZH (https://github.com/ShotokanZH/flipper_sub_plotters_comparers/blob/master/plotall.py)
def convert_file_to_array(f):
    y = []
    while True:
        line = f.readline()
        if not line:
            break
        if line.startswith("RAW_Data: "):
            for d in line.split(" ")[1:]:
                d = int(d)
                y.append(d)
                #print(f"{d}")
    return y

def analyze_all(all_data):
    stats_burst_nb = 0
    stats_bytes = []
    while(True):
        b, i_end, burst_found = get_burst(all_data)
        #print(f"i_end = {i_end}")
        if i_end == 0 or not burst_found:
            break
        print(f"Burst found: {b}")
        stats_burst_nb += 1
        try:
            data = decode_burst(b)
        except BaseException as e:
            print(f"This burst couldn't not be decoded: {e}")
            all_data = all_data[i_end::]
            continue
        data_bytes = convert_to_bytes(data)
        if len(data_bytes) > 0:
            print(f"In bytes:")
            print(", ".join("{:02X}".format(v) for v in data_bytes))
            stats_bytes.append(data_bytes)
        all_data = all_data[i_end::]
    print("--- STATS")
    print(f"--- Number of bursts: {stats_burst_nb}")
    print("--- Bytes:")
    for b in stats_bytes:
        print("------ " + ", ".join("{:02X}".format(v) for v in b))

def get_burst(y):
    #print(f"{y}")
    burst = []
    burst_found = False
    i = 0
    # search burst
    for v in y:
        i += 1
        #print(f"get_burst: analyzing {v}")
        if v >= 1700 and v < 1800:
            burst.append(v)
            burst_found = True
            #print(f"Found beginning of a burst: {v}")
            continue
        if burst_found and v < -1000000:
            #print(f"Found end of burst: {v}")
            break
        if burst_found:
            burst.append(v)
            #print(f"Append {v}")
    return burst, i, burst_found

def decode_burst(y):
    data = []
    preamble = []
    preamble_ended = False
    symbols = []
    symbols_split = []
    for v in y:
        if v <= -4000 and v > -5000:
            preamble_ended = True
            preamble.append(v)
            continue
        if not preamble_ended:
            preamble.append(v)
        if preamble_ended:
            symbols.append(v)

    print(f"Preamble = {preamble}")
    print(f"Symbols = {symbols}")

    for v in symbols:
        decode_symbol(symbols_split, v)

    print(f"Symbols_split = {symbols_split}")

    l = len(symbols_split)
    print(f"Length Symbols = {l}")
    odd_len = False
    if l % 2 != 0:
        odd_len = True
        #print(f"Odd number of symbols: {l}")

    for v in range(0, int(l)-1, 2):
        index = v
        a = symbols_split[index]
        b = symbols_split[index+1]
        #print(f"{a} {b}")
        data.append(decode_manchester(a, b))
    if odd_len:
        last = symbols_split[-1]
        if last == 1:
            data.append(1)
            print(f"Last symbol {last} converted to 1 (because the following symbol is a zero merged with the long zero pause which follows the burst")
        else:
            #print(f"Last symbol {last} could not be used...")
            raise BaseException(f"Odd number of symbols: {l} and last symbol {last} is not the beginning of a falling edge")

    print(f"Data = {data}")
    print(f"Length data = {len(data)}")
    return data

def decode_symbol(a_list, v):
    if v >= 350 and v <= 455:
        a_list.append(1)
        #print(f"Symbol {v} converted to 1")
    elif v <= -350 and v >= -455:
        a_list.append(0)
        #print(f"Symbol {v} converted to 0")
    elif v >= 700 and v <= 910:
        a_list.append(1)
        a_list.append(1)
        #print(f"Symbol {v} converted to 11")
    elif v <= -700 and v >= -910:
        a_list.append(0)
        a_list.append(0)
        #print(f"Symbol {v} converted to 00")
    else:
        raise BaseException(f"Symbol {v} unknown")

def decode_manchester(a, b):
    if a == 0 and b == 1:
        return 0
    elif a == 1 and b == 0:
        return 1
    else:
        raise BaseException(f"Symbols {a} {b} incompatible with Manchester coding")

def convert_to_bytes(data_bits):
    if len(data_bits) % 8 != 0:
        print("Data isn't aligned on byte length")
        return []
    data_bytes = [int("".join(map(str, data_bits[i:i+8])), 2) for i in range(0, len(data_bits), 8)]
    return data_bytes

def main():
    parser = argparse.ArgumentParser(description="Flipper file RAW analyzer based on Manchester decoding")
    parser.add_argument(
        "filename", help=".sub file(s) to be analyzed", type=str, nargs="+")
    args = parser.parse_args()
    try:
        for fname in args.filename:
            with open(fname, "r") as f:
                print(f"File: {fname}")
                y = convert_file_to_array(f)
                analyze_all(y)
    except Exception as e:
        exc_type, exc_obj, exc_tb = sys.exc_info()
        fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
        print(locals())
        print("ERROR:", e)
        print(exc_type, fname, exc_tb.tb_lineno)


if __name__ == '__main__':
    main()
