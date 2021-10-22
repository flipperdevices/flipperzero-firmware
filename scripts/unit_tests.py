#!/usr/bin/env python3

from flipper.storage import FlipperStorage

import logging
import argparse
import os
import sys
import binascii
import posixpath
import filecmp
import tempfile
import re
import serial
import serial.tools.list_ports


class Main:
    def __init__(self):
        # command args
        self.parser = argparse.ArgumentParser()
        self.parser.add_argument("-p", "--port", help="Port", required=False)

    def __call__(self):
        self.args = self.parser.parse_args()
        if self.args.port is None:
            self.detect_flipper_port()

        answer = self.run_unit_tests()
        result = answer.split("[UNIT_TESTS]: ", 1)[1]

        if result == "PASSED":
            exit(0)

        self.print_failed_tests(answer)
        if result != "FAILED":
            print(result)
        exit(1)

    def detect_flipper_port(self):
        ports = serial.tools.list_ports.grep("VID:PID=0483:5740")
        for port in ports:
            self.args.port = port.device
            return

    def run_unit_tests(self):
        storage = FlipperStorage(self.args.port)
        storage.start()
        storage.send_and_wait_eol("unit_tests\r")
        answer = storage.read.until(FlipperStorage.CLI_PROMPT).decode().strip()
        answer = self.escape_ansi(answer)
        storage.stop()
        return answer

    def escape_ansi(self, text):
        ansi_escape = re.compile(r"(?:\x1B[@-_]|[\x80-\x9F])[0-?]*[ -/]*[@-~]")
        return ansi_escape.sub("", text)

    def print_failed_tests(self, report):
        failed_test = r"(.*?) failed:\n\t(.*?):([0-9]+): (.*?)\n"
        matches = re.finditer(failed_test, report, re.MULTILINE)
        for m in matches:
            path = m.group(2)
            idx = path.find("applications/")
            if idx != -1:
                path = path[idx:]
            print(
                f"::error file={path},line={m.group(3)}::"
                f"{m.group(1)} failed: {m.group(4)}"
            )


if __name__ == "__main__":
    Main()()
