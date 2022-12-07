#!/usr/bin/env python3
import logging
import socket
import subprocess
import time

from flipper.assets.obdata import (
    OptionBytesData,
    ObReferenceValues,
    ObReferenceValuesGenerator,
)
from flipper.app import App


class OpenOCD:
    """OpenOCD cli wrapper"""

    COMMAND_TOKEN = "\x1a"

    def __init__(self, config: dict = {}) -> None:
        assert isinstance(config, dict)

        # Params base
        self.params = []

        self.gdb_port = 3333
        self.telnet_port = 4444
        self.tcl_port = 6666

        # Port
        if "port_base" in config and config["port_base"]:
            self.gdb_port = config["port_base"]
            self.tcl_port = config["port_base"] + 1
            self.telnet_port = config["port_base"] + 2

        self._add_command(f"gdb_port {self.gdb_port}")
        self._add_command(f"tcl_port {self.tcl_port}")
        self._add_command(f"telnet_port {self.telnet_port}")

        # Config files
        self._add_file("interface/cmsis-dap.cfg")
        self._add_file("target/stm32wbx.cfg")

        # DAP settings
        if "serial" in config and config["serial"]:
            self._add_command(f"cmsis_dap_serial {config['serial']}")

        # Other params
        if "params" in config:
            self.params += config["params"]

        # logging
        self.logger = logging.getLogger()

    def _add_command(self, command: str):
        self.params.append("-c")
        self.params.append(command)

    def _add_file(self, file: str):
        self.params.append("-f")
        self.params.append(file)

    def start(self, args: list[str] = []):
        """Start OpenOCD process"""

        params = ["openocd", *self.params, *args]
        self.logger.debug(f"_execute: {params}")
        self.process = subprocess.Popen(
            params, stderr=subprocess.PIPE, stdout=subprocess.PIPE
        )

        self._wait_for_openocd_tcl()

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("127.0.0.1", self.tcl_port))

    def _wait_for_openocd_tcl(self):
        """Wait for OpenOCD to start"""

        while True:
            stderr = self.process.stderr
            if not stderr:
                break
            line = stderr.readline()
            if not line:
                break
            line = line.decode("utf-8").strip()
            self.logger.debug(f"OpenOCD: {line}")
            if "Listening on port" in line and "for tcl connections" in line:
                break

    def stop(self):
        self.send_tcl("exit")
        self.send_tcl("shutdown")
        self.socket.close()
        try:
            self.process.wait(timeout=10)
        except subprocess.TimeoutExpired:
            self.process.kill()

    def send_tcl(self, cmd) -> str:
        """Send a command string to TCL RPC. Return the result that was read."""

        try:
            self.logger.debug(cmd)
            data = (cmd + OpenOCD.COMMAND_TOKEN).encode("utf-8")
            self.logger.debug(f"<- {data}")

            self.socket.send(data)
        except:
            self.logger.error("Failed to send command to OpenOCD")
            self.postmortem()
            raise

        try:
            data = self._recv()
            return data
        except Exception as e:
            self.logger.error("Failed to receive response from OpenOCD")
            self.logger.error(e)
            self.postmortem()
            raise

    def _recv(self):
        """Read from the stream until the token (\x1a) was received."""
        # TODO: timeout
        data = bytes()
        while True:
            chunk = self.socket.recv(4096)
            data += chunk
            if bytes(OpenOCD.COMMAND_TOKEN, encoding="utf-8") in chunk:
                break

        self.logger.debug(f"-> {data}")

        data = data.decode("utf-8").strip()
        data = data[:-1]  # strip trailing \x1a

        return data

    def postmortem(self) -> None:
        """Postmortem analysis of the OpenOCD process"""
        stdout, stderr = self.process.communicate()

        log = self.logger.error
        if self.process.returncode == 0:
            log = self.logger.debug
            log("OpenOCD exited normally")
        else:
            log("OpenOCD exited with error")

        log(f"Exit code: {self.process.returncode}")
        for line in stdout.decode("utf-8").splitlines():
            log(f"Stdout: {line}")

        for line in stderr.decode("utf-8").splitlines():
            log(f"Stderr: {line}")

    def read_32(self, addr: int) -> int:
        """Read 32-bit value from memory"""
        data = self.send_tcl(f"mdw {addr}").strip()
        data = data.split(" ")[-1]
        data = int(data, 16)
        return data


class Main(App):
    OPTION_BYTE_BASE = 0x1FFF8000

    def init(self):
        # Subparsers
        self.subparsers = self.parser.add_subparsers(help="sub-command help")

        # Check command
        self.parser_check = self.subparsers.add_parser(
            "check", help="Check Option Bytes"
        )
        self._add_args(self.parser_check)
        self.parser_check.set_defaults(func=self.check)

        # Set command
        self.parser_set = self.subparsers.add_parser("set", help="Set Option Bytes")
        self._add_args(self.parser_set)
        self.parser_set.set_defaults(func=self.set)

    def _add_args(self, parser):
        parser.add_argument("--port-base", type=int, help="OpenOCD port base")
        parser.add_argument("--serial", type=str, help="DAP-Link Serial Number")
        parser.add_argument(
            "--ob-path", type=str, help="DAP-Link Serial Number", default="ob.data"
        )

    def _get_dap_params(self):
        return {
            "serial": self.args.serial,
            "port_base": self.args.port_base,
        }

    def check(self):
        self.logger.info(f"Checking Option Bytes")

        # OpenOCD
        oocd = OpenOCD(self._get_dap_params())
        oocd.start()

        oocd.send_tcl("reset init")

        # Generate Option Bytes data
        ob_data = OptionBytesData(self.args.ob_path)
        ob_values = ob_data.gen_values().export()
        ob_reference = ob_values.reference
        ob_compare_mask = ob_values.compare_mask
        ob_length = len(ob_reference)
        ob_words = int(ob_length / 4)

        # Read Option Bytes
        ob_read = bytes()
        for i in range(ob_words):
            addr = self.OPTION_BYTE_BASE + i * 4
            value = oocd.read_32(addr)
            ob_read += value.to_bytes(4, "little")

        # Compare Option Bytes with reference by mask
        ob_compare = bytes()
        for i in range(ob_length):
            ob_compare += bytes([ob_read[i] & ob_compare_mask[i]])

        # Compare Option Bytes
        if ob_reference == ob_compare:
            self.logger.info("Option Bytes are valid")
            return_code = 0
        else:
            self.logger.error("Option Bytes are invalid")
            self.logger.error(f"Reference: {ob_reference.hex()}")
            self.logger.error(f"Compare:   {ob_compare.hex()}")
            return_code = 1

        oocd.send_tcl("reset run")
        oocd.stop()

        return return_code

    def set(self):
        self.logger.error(f"TODO: Set Option Bytes")


if __name__ == "__main__":
    Main()()
