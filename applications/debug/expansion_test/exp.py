#!/usr/bin/env python3

import serial
from struct import pack, unpack
from enum import Enum

PORT = "/dev/cu.usbmodem1101"
EXPANSION_DEFAULT_BAUD_RATE = 9600

class ExpansionFrameType(Enum):
    Heartbeat = b'\x01'
    Status = b'\x02'
    BaudRate = b'\x03'
    Control = b'\x04'
    Data = b'\x05'

class ExpansionFrameError(Enum):
    None_ = b'\x00'
    Unknown = b'\x01'
    BaudRate = b'\x02'

class ExpansionFrameControlCommand(Enum):
    StartRpc = b'\x00'
    StopRpc = b'\x01'

def send_frame(ser: serial.Serial, frame_type: ExpansionFrameType, data: bytes):
    print("Sending frame of type {} with data {}, frame: '{}'".format(frame_type, data, frame_type.value + data))
    ser.write(frame_type.value)
    ser.write(data)

def send_pulse(ser: serial.Serial):
    ser.write(b'\xAA')

def receive_ready(ser: serial.Serial):
    resp = ser.read(1)
    if resp == ExpansionFrameType.Heartbeat.value:
        return True
    else:
        return False
    
def send_baud_rate(ser: serial.Serial, baud_rate: int):
    send_frame(ser, ExpansionFrameType.BaudRate, pack('<I', baud_rate))

def receive_status(ser: serial.Serial):
    resp = ser.read(1)
    if resp == ExpansionFrameType.Status.value:
        resp = ser.read(1)
        if resp == ExpansionFrameError.None_.value:
            return True
        else:
            print("Expected {} but received {}".format(ExpansionFrameError.None_.value, resp))
            return False
    else:
        print("Expected {} but received {}".format(ExpansionFrameType.Status.value, resp))
        return False
    
def main():
    ser = serial.Serial(PORT, EXPANSION_DEFAULT_BAUD_RATE)

    ser.reset_input_buffer()

    # Send pulses to emulate module insertion
    send_pulse(ser)

    # Wait for ExpansionFrameTypeHeartbeat packet
    if receive_ready(ser):
        print("Expansion protocol detected")
    else:
        print("Expansion protocol not detected")

    # Send baud rate
    send_baud_rate(ser, 115200)

    if receive_status(ser):
        print("Baud rate set")
    else:
        print("Baud rate not set")
    

if __name__ == "__main__":
    main()