# CAME generator for given 12bits keys
#
# Code reuse for CAME protocol from
# https://github.com/UberGuidoZ/Flipper/blob/main/Sub-GHz/Garages/CAME_brute_force/12Bit/433.92Mhz/CAMEbruteforcer433.py
# and https://github.com/UberGuidoZ/Flipper/blob/main/Sub-GHz/Garages/CAME_brute_force/12Bit/868.35Mhz/CAMEbruteforcer868.py
# and also, for the other protocols:
# https://github.com/tobiabocchi/flipperzero-bruteforce/blob/main/flipperzero-bruteforce.py

import os

# Script settings:
repetition = 3
n_bits = 12
file_header = """
Filetype: Flipper SubGhz RAW File
Version: 1
Frequency: {}
Preset: FuriHalSubGhzPresetOok650Async
Protocol: RAW
"""
file_header_433 = file_header.format("433920000")
file_header_868 = file_header.format("868350000")

# Protocol settings: https://phreakerclub.com/447
combos = [2218,  # 100010101010
        1361] # 010101010001

protocols = {
    "CAME": {
        "signal": 250,
        "transposition_table": {
            "0": "-250 500 ",
            "1": "-500 250 ",
        }
    },
    "NICE": {
        "signal": 700,
        "transposition_table": {
            "0": "-700 1400 ",
            "1": "-1400 700 ",
        }
    },

}

for key_dec in combos:
    key_bin = f"{key_dec:0{n_bits}b}"  # format as 12 digits bin
    print(f"keybin = {key_bin}")

    for proto in protocols:
        signal = protocols[proto]["signal"]
        transposition_table = protocols[proto]["transposition_table"]
        Pilot = -36*signal
        pilot_period = f"{Pilot} {signal} "
        key_str = pilot_period

        for bit in key_bin:
            key_str += transposition_table[bit]
        joined = "".join(key_str)
        key_str = key_str * repetition

        # 433
        filename = f"{proto}_433_{key_dec}.sub"
        with open(filename, "w") as f:
            f.write(file_header_433)
        with open(filename, "a") as f:
            f.write("RAW_Data: " + key_str + "\n")

        # 868
        filename = f"{proto}_868_{key_dec}.sub"
        with open(filename, "w") as f:
            f.write(file_header_868)
        with open(filename, "a") as f:
            f.write("RAW_Data: " + key_str + "\n")

