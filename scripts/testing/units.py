#!/usr/bin/env python3
import logging
import re
import sys

import serial
from await_flipper import flp_serial_by_name


def main():
    logging.basicConfig(
        format="%(asctime)s %(levelname)-8s %(message)s",
        level=logging.INFO,
        datefmt="%Y-%m-%d %H:%M:%S",
    )
    logging.log(logging.INFO, "Trying to run units on flipper")
    flp_serial = flp_serial_by_name(sys.argv[1])

    if flp_serial == "":
        logging.log(logging.ERROR, "Flipper not found!")
        sys.exit(1)

    with serial.Serial(flp_serial, timeout=1) as flipper:
        logging.log(logging.INFO, f"Found Flipper at {flp_serial}")
        flipper.baudrate = 230400
        flipper.flushOutput()
        flipper.flushInput()

        flipper.timeout = 180

        flipper.read_until(b">: ").decode("utf-8")
        flipper.write(b"unit_tests\r")
        data = flipper.read_until(b">: ").decode("utf-8")

        lines = data.split("\r\n")

        tests_re = r"Failed tests: \d{0,}"
        time_re = r"Consumed: \d{0,}"
        leak_re = r"Leaked: \d{0,}"
        status_re = r"Status: \w{3,}"

        tests_pattern = re.compile(tests_re)
        time_pattern = re.compile(time_re)
        leak_pattern = re.compile(leak_re)
        status_pattern = re.compile(status_re)

        tests, time, leak, status = None, None, None, None
        total = 0

        for line in lines:
            logging.log(logging.INFO, line)
            if "()" in line:
                total += 1

            if not tests:
                tests = re.match(tests_pattern, line)
            if not time:
                time = re.match(time_pattern, line)
            if not leak:
                leak = re.match(leak_pattern, line)
            if not status:
                status = re.match(status_pattern, line)

        if leak is None or time is None or leak is None or status is None:
            logging.log(logging.ERROR, "Failed to parse output")
            sys.exit(1)

        leak = int(re.findall(r"[- ]\d+", leak.group(0))[0])
        status = re.findall(r"\w+", status.group(0))[1]
        tests = int(re.findall(r"\d+", tests.group(0))[0])
        time = int(re.findall(r"\d+", time.group(0))[0])

        if tests > 0 or status != "PASSED":
            logging.log(logging.ERROR, f"Got {tests} failed tests.")
            logging.log(logging.ERROR, f"Leaked (not failing on this stat): {leak}")
            logging.log(logging.ERROR, f"Status: {status}")
            logging.log(logging.ERROR, f"Time: {time/1000} seconds")
            sys.exit(1)

        logging.log(logging.INFO, f"Leaked (not failing on this stat): {leak}")
        logging.log(
            logging.INFO,
            f"Tests ran successfully! Time elapsed {time/1000} seconds. Passed {total} tests.",
        )

        sys.exit(0)


if __name__ == "__main__":
    main()
