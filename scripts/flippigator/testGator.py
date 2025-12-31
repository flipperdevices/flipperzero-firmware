#!/usr/bin/env python3

import os
import sys
import time

import serial
from flippigator.flippigator import Gator, Navigator
from termcolor import colored


def main():  # TODO: to async
    print(colored("Let's go!", "magenta"))

    gator_serial = serial.Serial(sys.argv[1], timeout=1)
    gator_serial.baudrate = 115200

    time.sleep(3)

    gator_serial.flushOutput()
    gator_serial.flushInput()

    gator = Gator(gator_serial, 900, 900)
    gator.home()


if __name__ == "__main__":
    main()
