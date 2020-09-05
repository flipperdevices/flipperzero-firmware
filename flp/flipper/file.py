from .funcs import *
from .protocol import *
import serial


class RemoteIOError(IOError):
    pass


def push(ser: serial.Serial, local_path, remote_path):
    with open(local_path, "rb") as f:  # Reading local file
        a = f.read()

    f_open(ser, remote_path, "w")
    if not f_valid(ser):  # Failing if f_open is failed on device's side
        raise RemoteIOError
    f_write(ser, a)

    f_close(ser)


def pull(ser: serial.Serial, remote_path, local_path):
    f_open(ser, remote_path, "r")
    if not f_valid(ser):
        raise RemoteIOError

    with open(local_path, "wb") as f:
        f.write(f_readall(ser))

    f_close(ser)
