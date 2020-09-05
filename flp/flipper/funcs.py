from .protocol import *
import serial


def f_readall(ser: serial.Serial) -> bytes:
    """
    Function to simplify reading of file over serial.

    :param ser: Serial port object
    :return: content of file represented in `bytes`
    """
    size = f_size(ser)
    a = bytearray()
    seek = 0
    while seek < 128 * (size // 128 - 1):  # Reading in blocks of 128
        seek += 128
        f_lseek(ser, seek)
        a.extend(f_read(ser, 128))

    seek += 128
    f_lseek(ser, seek)
    a.extend(f_read(ser, size - seek))  # Appending mod

    f_lseek(ser, 0)  # Resetting seek to default-position

    return bytes(a)
