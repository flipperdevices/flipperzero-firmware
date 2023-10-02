import sys

def main():
    info = 0x0D94C5EC007F1991
    te = 200 #200 is what original remote used.  (How short the pulses are, smaller is faster.)
    hc = 11  #11 is what original remote sent.   (How many pulses in header.  You may be able to reduce this.)
    end = 40 #40 is what original remote sent.   (How long the silence guard is.  You may be able to reduce this.)
    repeat=5 #5 is what original remote sent.    (How many times to repeat the signal.  You may be able to reduce this.)

    with open('genie.sub', 'w') as f:

        f.write("Filetype: Flipper SubGhz RAW File\n")
        f.write("Version: 1\n")
        f.write("Frequency: 315000000\n")
        f.write("Preset: FuriHalSubGhzPresetOok650Async\n")
        f.write("Protocol: RAW\n")

        for j in range(repeat):
            f.write(f"RAW_Data: ")

            # header
            for i in range(hc):
                f.write(f"{te} -{te} ")
            f.write(f"{te} -{9*te} ")

            # iterate over each bit
            for i in range(64):
                # get bit value
                bit = (info >> (63 - i)) & 1

                # get bit value, output on a single line.
                if bit == 0:
                    f.write(f"{2*te} -{te} ")
                else:
                    f.write(f"{te} -{2*te} ")

            # stop
            f.write(f"{te} -{2*te} {te} -{end*te}\n")

main()