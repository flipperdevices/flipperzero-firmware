#!/usr/bin/env python3
import re
import time
from datetime import datetime

from serial.serialutil import SerialException

from flipper.app import App
from flipper.storage import FlipperStorage, FlipperStorageException
from flipper.utils.cdc import resolve_port


class Main(App):
    def __init__(self, no_exit=False):
        super().__init__(no_exit)
        self.test_results = None

    def init(self):
        self.parser.add_argument("-p", "--port", help="CDC Port", default="auto")
        self.parser.add_argument(
            "-t", "--timeout", help="Timeout in seconds", type=int, default=10
        )

        self.subparsers = self.parser.add_subparsers(help="sub-command help")

        self.parser_await_flipper = self.subparsers.add_parser(
            "await_flipper", help="Wait for Flipper to connect or reconnect"
        )
        self.parser_await_flipper.set_defaults(func=self.await_flipper)

        self.parser_run_units = self.subparsers.add_parser(
            "run_units", help="Run unit tests and post result"
        )
        self.parser_run_units.set_defaults(func=self.run_units)

    # A port can open while the CLI never answers, e.g. mid-update.
    START_TIMEOUT = 10

    def _get_flipper(self, timeout: int):
        deadline = time.monotonic() + timeout
        attempt = 0
        while time.monotonic() < deadline:
            attempt += 1
            time.sleep(1)
            if not (port := resolve_port(self.logger, self.args.port)):
                self.logger.info(f"Failed to find flipper (Attempt {attempt}).")
                continue

            flipper = FlipperStorage(port)
            try:
                flipper.start(timeout=self.START_TIMEOUT)
                self.logger.info(f"Flipper successfully started at {port}.")
                return flipper
            except (IOError, FlipperStorageException) as e:
                flipper.stop()
                self.logger.info(f"Failed to start flipper (Attempt {attempt}): {e}")

        self.logger.error(f"Flipper failed to start within {timeout}s.")
        return None

    def await_flipper(self):
        if not (flipper := self._get_flipper(timeout=self.args.timeout)):
            return 1

        self.logger.info("Flipper started")
        flipper.stop()
        return 0

    def run_units(self):
        if not (flipper := self._get_flipper(timeout=10)):
            return 1

        self.logger.info("Running unit tests")
        flipper.send("unit_tests" + "\r")
        self.logger.info("Waiting for unit tests to complete")

        tests, elapsed_time, leak, status = None, None, None, None
        total = 0
        all_required_found = False

        full_output = []

        tests_pattern = re.compile(r"Failed tests: \d{0,}")
        time_pattern = re.compile(r"Consumed: \d{0,}")
        leak_pattern = re.compile(r"Leaked: \d{0,}")
        status_pattern = re.compile(r"Status: \w{3,}")

        try:
            while not all_required_found:
                try:
                    line = flipper.read.until("\r\n", cut_eol=True).decode()
                    self.logger.info(line)
                    if "command not found," in line:
                        self.logger.error(f"Command not found: {line}")
                        return 1

                    if "()" in line:
                        total += 1
                        self.logger.debug(f"Test completed: {line}")

                    if not tests:
                        tests = tests_pattern.match(line)
                    if not elapsed_time:
                        elapsed_time = time_pattern.match(line)
                    if not leak:
                        leak = leak_pattern.match(line)
                    if not status:
                        status = status_pattern.match(line)

                    pattern = re.compile(
                        r"(\[-]|\[\\]|\[\|]|\[/-]|\[[^\]]*\]|\x1b\[\d+D)"
                    )
                    line_to_append = pattern.sub("", line)
                    pattern = re.compile(r"\[3D[^\]]*")
                    line_to_append = pattern.sub("", line_to_append)
                    line_to_append = f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S,%f')} {line_to_append}"

                    full_output.append(line_to_append)

                    if tests and elapsed_time and leak and status:
                        all_required_found = True
                        try:
                            remaining = flipper.read.until(">: ", cut_eol=True).decode()
                            if remaining.strip():
                                full_output.append(remaining)
                        except:
                            pass
                        break

                except Exception as e:
                    self.logger.error(f"Error reading output: {e}")
                    raise

            if None in (tests, elapsed_time, leak, status):
                raise RuntimeError(
                    f"Failed to parse output: {tests} {elapsed_time} {leak} {status}"
                )

            leak = int(re.findall(r"[- ]\d+", leak.group(0))[0])
            status = re.findall(r"\w+", status.group(0))[1]
            tests = int(re.findall(r"\d+", tests.group(0))[0])
            elapsed_time = int(re.findall(r"\d+", elapsed_time.group(0))[0])

            test_results = {
                "full_output": "\n".join(full_output),
                "total_tests": total,
                "failed_tests": tests,
                "elapsed_time_ms": elapsed_time,
                "memory_leak_bytes": leak,
                "status": status,
            }

            self.test_results = test_results

            output_file = "unit_tests_output.txt"
            with open(output_file, "w") as f:
                f.write(test_results["full_output"])

            print(
                f"::notice:: Total tests: {total} Failed tests: {tests} Status: {status} Elapsed time: {elapsed_time / 1000} s Memory leak: {leak} bytes"
            )

            if tests > 0 or status != "PASSED":
                self.logger.error(f"Got {tests} failed tests.")
                self.logger.error(f"Leaked (not failing on this stat): {leak}")
                self.logger.error(f"Status: {status}")
                self.logger.error(f"Time: {elapsed_time / 1000} seconds")
                return 1

            self.logger.info(f"Leaked (not failing on this stat): {leak}")
            self.logger.info(
                f"Tests ran successfully! Time elapsed {elapsed_time / 1000} seconds. Passed {total} tests."
            )
            return 0

        finally:
            flipper.stop()


if __name__ == "__main__":
    Main()()
