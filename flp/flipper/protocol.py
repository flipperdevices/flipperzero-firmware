import serial

# TODO: Rewrite in OOP


def f_open(ser: serial.Serial, filename, mode="r"):
    ser.write(f"f_open {filename} {mode}\n".encode())


def f_valid(ser: serial.Serial):
    ser.write(b"f_valid\n")
    return bool(int(ser.read_until().decode()))


def f_write(ser: serial.Serial, data: bytes or bytearray):
    ser.write(f"f_write {len(data)}\n".encode())
    ser.write(data)
    ser.write(b"\n")


def f_close(ser: serial.Serial):
    ser.write(b"f_close\n")


def f_read(ser: serial.Serial, size: int) -> bytes:
    ser.write(f"f_read {size}\n".encode())
    return ser.read(size)


def f_lseek(ser: serial.Serial, size: int):
    ser.write(f"f_lseek {size}\n".encode())


def f_tell(ser: serial.Serial):
    ser.write(b"f_tell\n")
    return int(ser.read_until().decode())


def f_size(ser: serial.Serial):
    ser.write(b"f_size\n")
    return int(ser.read_until().decode())
