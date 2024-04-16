#!/usr/bin/env python3

import os
import sys
import time
import cv2

import serial
from flipperzero_protobuf_py.flipperzero_protobuf.flipper_base import (
    FlipperProtoException,
)
from flipperzero_protobuf_py.flipperzero_protobuf.flipper_proto import FlipperProto
from flippigator.flippigator import (
    FlipperHEXKeyboard,
    FlipperTextKeyboard,
    Gator,
    Navigator,
)

def printProgressBar (iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    """
    Call in a loop to create terminal progress bar
    @params:
        iteration   - Required  : current iteration (Int)
        total       - Required  : total iterations (Int)
        prefix      - Optional  : prefix string (Str)
        suffix      - Optional  : suffix string (Str)
        decimals    - Optional  : positive number of decimals in percent complete (Int)
        length      - Optional  : character length of bar (Int)
        fill        - Optional  : bar fill character (Str)
        printEnd    - Optional  : end character (e.g. "\r", "\r\n") (Str)
    """
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print(f'\r{prefix} |{bar}| {percent}% {suffix}', end = printEnd)
    # Print New Line on Complete
    if iteration == total: 
        print()

def main():
    print("Let's go!")

    flipper = serial.Serial(sys.argv[1], timeout=1)
    flipper.baudrate = 2304000
    flipper.flushOutput()
    flipper.flushInput()
    flipper.timeout = None

    proto = FlipperProto(serial_port=flipper, debug=True)

    print("Request RPC session")
    proto.start_rpc_session()
    print("RPC session started")

    nav = Navigator(proto)
    nav.update_screen()
    
    mifare = {
        "Name"          : "MIFARE Classic 1K",
        "UID"           : "A3 D7 E3 BB",
        "Keys"          : 32,
        "Keys found"    : 28,
        "Sectors"       : 16,
        "Read sectors"  : 14,
        "ATQA"          : "00 04",
        "SAK"           : "08",
        "Data"          :   (
            "A3D7 E3BB 2C08 0400 ",
            "6263 6465 6667 6869 ",
            "0100 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0200 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0400 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0500 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0600 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0800 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0900 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "1000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "???? ???? ???? ???? ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "0000 0000 0000 0000 ",
            "FFFF FFFF FFFF FF07 ",
            "8069 FFFF FFFF FFFF ",
            )
    }
    nav.nfc.read_mifare_classic(mifare)

    '''
    info = list()
    hexT = ['0', '1', '2', '3', '4', '5', '6', '7', '8' ,'9', 'A','B', 'C', 'D', 'E', 'F', '/', '?']

    infoStr = 128

    tempList = nav.get_ref_from_list(hexT, nav.font_profont11, invert=0, no_space=1)
    for k in range(int(infoStr/5)):
        for l in range(5):
            for i in range(4):
                for s in range(4):
                    info.append(nav.recog_ref(tempList, area = (4+11*l,15+11*l,3+s*6+i*5*6, 9+s*6+i*5*6))[0])
            printProgressBar(k*5+l, infoStr, prefix = 'Progress:', suffix = 'Complete', length = 50)
        for poshlo_ono_ko_vsem_chertyam in range(5):
            nav.press_down()
        nav.update_screen()

    for l in range(infoStr%5-1,5):
        for i in range(4):
            for s in range(4):
                info.append(nav.recog_ref(tempList, area = (4+11*l,15+11*l,3+s*6+i*5*6, 9+s*6+i*5*6))[0])
        printProgressBar(infoStr-infoStr%5-1+l, infoStr, prefix = 'Progress:', suffix = 'Complete', length = 50)    

    for k in range(infoStr):
        print("\"", end = '')
        for i in range(4):
            print(info[k*16+i*4+0], end='')
            print(info[k*16+i*4+1], end='')
            print(info[k*16+i*4+2], end='')
            print(info[k*16+i*4+3], end='')
            print(" ", end='')
        print("\",")

    nav.update_screen()
    time.sleep(1)
    '''
if __name__ == "__main__":
    main()
