#!/usr/bin/env python3

import serial
from struct import pack, unpack

PORT = "/dev/cu.usbmodem1101"
EXPANSION_DEFAULT_BAUD_RATE = 9600

def main():
    ser = serial.Serial(PORT, EXPANSION_DEFAULT_BAUD_RATE)

    ser.reset_input_buffer()

    # Send pulses to emulate module insertion
    ser.write(b'\xAA')

    resp = ser.read(1)

    # Wait for ExpansionFrameTypeHeartbeat packet
    if resp == b'\x01': 
        print("Expansion protocol detected")
    else:
        print("Expansion protocol not detected")
    
    

if __name__ == "__main__":
    main()