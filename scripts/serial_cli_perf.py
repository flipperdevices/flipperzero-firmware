import argparse
import logging
from serial import Serial
from random import randint
from time import time

from flipper.utils.cdc import resolve_port


def main():
    logger = logging.getLogger()
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", help="CDC Port", default="auto")
    parser.add_argument(
        "-l", "--length", type=int, help="Number of bytes to send", default=1024**2
    )
    args = parser.parse_args()

    if not (port := resolve_port(logger, args.port)):
        logger.error("Is Flipper connected via USB and not in DFU mode?")
        return 1
    port = Serial(port, 230400)
    port.timeout = 2

    port.read_until(b">: ")
    port.write(b"echo\r")
    port.read_until(b">: ")

    print(f"Transferring {args.length} bytes. Hang tight...")

    start_time = time()

    bytes_to_send = args.length
    block_size = 1024
    while bytes_to_send:
        actual_size = min(block_size, bytes_to_send)
        # can't use 0x03 because that's ASCII ETX, or Ctrl+C
        block = bytes([randint(4, 255) for _ in range(actual_size)])

        port.write(block)
        return_block = port.read(actual_size)

        if return_block != block:
            logger.error("Incorrect block received")
            break

        bytes_to_send -= actual_size

    end_time = time()
    delta = end_time - start_time
    speed = args.length / delta
    print(f"Speed: {speed/1024:.2f} KiB/s")

    port.write(b"\x03")  # Ctrl+C
    port.close()


if __name__ == "__main__":
    main()
