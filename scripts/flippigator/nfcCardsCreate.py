#!/usr/bin/env python3

import os
import sys
import time

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

    # NFC-A 7-bytes UID adding
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NFC-A 7-bytes UID")
    nav.press_ok()
    key = FlipperHEXKeyboard(nav)
    key.send("08")
    key.send("0014")
    key.send("00112233445566")
    key = FlipperTextKeyboard(nav)
    key.send("tN0")

    # NFC-A 7-bytes UID adding
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NFC-A 4-bytes UID")
    nav.press_ok()
    key = FlipperHEXKeyboard(nav)
    key.send("08")
    key.send("0014")
    key.send("00112233")
    key = FlipperTextKeyboard(nav)
    key.send("tN1")

    # Mifare Ultralight
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Ultralight")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN2")

    # Mifare Ultralight EV1 11
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Ultralight EV1 11")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN3")

    # Mifare Ultralight EV1 H11
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Ultralight EV1 H11")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN4")

    # Mifare Ultralight EV1 21
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Ultralight EV1 21")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN5")

    # Mifare Ultralight EV1 H21
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Ultralight EV1 H21")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN6")

    # NTAG203
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NTAG203")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN7")

    # NTAG213
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NTAG213")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN8")

    # NTAG215
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NTAG215")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN9")

    # NTAG216
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NTAG216")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN10")

    # NTAG I2C 1k
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NTAG I2C 1k")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN11")

    # NTAG I2C 2k
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NTAG I2C 2k")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN12")

    # NTAG I2C Plus 1k
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NTAG I2C Plus 1k")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN13")

    # NTAG I2C Plus 2k
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("NTAG I2C Plus 2k")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN14")

    # Mifare Mini
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Mini")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN15")

    # Mifare Classic 1k 4byte UID
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Classic 1k 4byte UID")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN16")

    # Mifare Classic 1k 7byte UID
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Classic 1k 4byte UID")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN17")

    # Mifare Classic 4k 4byte UID
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Classic 1k 4byte UID")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN18")

    # Mifare Classic 4k 4byte UID
    nav.nfc.go_into()
    nav.go_to("Add Manually")
    nav.press_ok()
    nav.go_to("Mifare Classic 1k 4byte UID")
    nav.press_ok()
    nav.press_right()
    nav.go_to("Save")
    nav.press_ok()
    key = FlipperTextKeyboard(nav)
    key.send("tN19")


if __name__ == "__main__":
    main()
