import sys

def int_to_bytes(x, r):
    return [(x >> i*8) & 0xff for i in range(r-1, -1, -1)]

def main():
    major = 2
    minor = 4

    counter = 0
    counter_max = 0

    with open('keys.txt', 'r') as fkeys:

        while True:
            line = fkeys.readline()
            if not line:
                break
            split = line.split(sep=",")
            if (len(split) == 2):
                line = split[1]
            line = line.strip(" \n\r")
            high = line[0:8]
            low = line[8:16]
            high_int = int(high, 16)
            low_int = int(low, 16)

            print(f"line: {line} {high} {low}")

            try:
                with open(f'{low}.gne', 'rb') as fin:
                    fin.close()
            except:
                with open(f'{low}.gne', 'wb') as fout:
                    fout.write(bytes([0x47, 0x2A]))
                    fout.write(bytes([major, minor]))
                    fout.write(bytes(int_to_bytes(low_int, 4)))
                    fout.write(bytes([0,0,0,0,0,0,0,0]))
                    for i in range(65536):
                        fout.write(bytes([0,0,0,0])) 
                    fout.flush()
                    fout.close()
                    print ('created file')
                pass
            
            with open(f'{low}.gne', 'r+b') as fout:
                fout.seek(16+((counter&0xFFFF)*4))
                fout.write(bytes(int_to_bytes(high_int, 4)))
                counter = counter + 1
                fout.seek(10)
                counter_max = max(counter&0xFFFF, counter_max)
                fout.write(bytes(int_to_bytes(counter_max,2)))
                fout.seek(16+(65536*4))
                fout.flush()
                fout.close()

main()