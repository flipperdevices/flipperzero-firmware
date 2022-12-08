#!/usr/bin/env python3
import logging
import socket
import subprocess

from dataclasses import dataclass

from flipper.assets.obdata import (
    OptionBytesData,
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
        if port_base := config.get("port_base", None):
            self.gdb_port = port_base
            self.tcl_port = port_base + 1
            self.telnet_port = port_base + 2

        self._add_command(f"gdb_port {self.gdb_port}")
        self._add_command(f"tcl_port {self.tcl_port}")
        self._add_command(f"telnet_port {self.telnet_port}")

        # Config files
        self._add_file("interface/cmsis-dap.cfg")
        self._add_file("target/stm32wbx.cfg")

        # DAP settings
        if serial := config.get("serial", None):
            self._add_command(f"cmsis_dap_serial {serial}")

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
        # TODO: timeout
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
        data = data.split(": ")[-1]
        data = int(data, 16)
        return data

    def write_32(self, addr: int, value: int) -> None:
        """Write 32-bit value to memory"""
        self.send_tcl(f"mww {addr} {value}")


@dataclass
class RegisterBitDefinition:
    name: str
    offset: int
    size: int
    value: int = 0


class Register32:
    def __init__(self, address: int, definition_list: list[RegisterBitDefinition]):
        self.__dict__["names"] = [definition.name for definition in definition_list]
        self.names = [definition.name for definition in definition_list]  # typecheck
        self.address = address
        self.definition_list = definition_list

        # Validate that the definitions are not overlapping
        for i in range(len(definition_list)):
            for j in range(i + 1, len(definition_list)):
                if self._is_overlapping(definition_list[i], definition_list[j]):
                    raise ValueError("Register definitions are overlapping")

    def _is_overlapping(
        self, a: RegisterBitDefinition, b: RegisterBitDefinition
    ) -> bool:
        if a.offset + a.size <= b.offset:
            return False
        if b.offset + b.size <= a.offset:
            return False
        return True

    def _get_definition(self, name: str) -> RegisterBitDefinition:
        for definition in self.definition_list:
            if definition.name == name:
                return definition
        raise ValueError(f"Register definition '{name}' not found")

    def get_definition_list(self) -> list[RegisterBitDefinition]:
        return self.definition_list

    def get_address(self) -> int:
        return self.address

    def set_reg_value(self, name: str, value: int):
        definition = self._get_definition(name)
        if value > (1 << definition.size) - 1:
            raise ValueError(
                f"Value {value} is too large for register definition '{name}'"
            )
        definition.value = value

    def get_reg_value(self, name: str) -> int:
        definition = self._get_definition(name)
        return definition.value

    def __getattr__(self, attr):
        if str(attr) in self.names:
            return self.get_reg_value(str(attr))
        else:
            return self.__dict__[attr]

    def __setattr__(self, attr, value):
        if str(attr) in self.names:
            self.set_reg_value(str(attr), value)
        else:
            self.__dict__[attr] = value

    def __dir__(self):
        return self.names

    def set(self, value: int):
        for definition in self.definition_list:
            definition.value = (value >> definition.offset) & (
                (1 << definition.size) - 1
            )

    def get(self) -> int:
        value = 0
        for definition in self.definition_list:
            value |= definition.value << definition.offset
        return value

    def load(self, openocd: OpenOCD):
        self.set(openocd.read_32(self.address))

    def store(self, openocd: OpenOCD):
        openocd.write_32(self.address, self.get())


class STM32WB55:
    # Address of Option byte in flash
    OPTION_BYTE_BASE = 0x1FFF8000

    # Flash base address
    FLASH_BASE = 0x58004000

    # Flash unlock register
    FLASH_KEYR = FLASH_BASE + 0x08

    # Option byte unlock register
    FLASH_OPTKEYR = FLASH_BASE + 0x0C

    # Flash unlock keys
    FLASH_UNLOCK_KEY1 = 0x45670123
    FLASH_UNLOCK_KEY2 = 0xCDEF89AB

    # Option byte unlock keys
    FLASH_UNLOCK_OPTKEY1 = 0x08192A3B
    FLASH_UNLOCK_OPTKEY2 = 0x4C5D6E7F

    # Flash control register
    FLASH_CR = Register32(
        FLASH_BASE + 0x14,
        [
            RegisterBitDefinition("PG", 0, 1),
            RegisterBitDefinition("PER", 1, 1),
            RegisterBitDefinition("MER", 2, 1),
            RegisterBitDefinition("PNB", 3, 8),
            RegisterBitDefinition("_", 11, 5),
            RegisterBitDefinition("STRT", 16, 1),
            RegisterBitDefinition("OPT_STRT", 17, 1),
            RegisterBitDefinition("FSTPG", 18, 1),
            RegisterBitDefinition("_", 19, 5),
            RegisterBitDefinition("EOPIE", 24, 1),
            RegisterBitDefinition("ERRIE", 25, 1),
            RegisterBitDefinition("RD_ERRIE", 26, 1),
            RegisterBitDefinition("OBL_LAUNCH", 27, 1),
            RegisterBitDefinition("_", 28, 2),
            RegisterBitDefinition("OPT_LOCK", 30, 1),
            RegisterBitDefinition("LOCK", 31, 1),
        ],
    )

    # Flash status register
    FLASH_SR = Register32(
        FLASH_BASE + 0x10,
        [
            RegisterBitDefinition("EOP", 0, 1),
            RegisterBitDefinition("OP_ERR", 1, 1),
            RegisterBitDefinition("_", 2, 1),
            RegisterBitDefinition("PROG_ERR", 3, 1),
            RegisterBitDefinition("WRP_ERR", 4, 1),
            RegisterBitDefinition("PGA_ERR", 5, 1),
            RegisterBitDefinition("SIZE_ERR", 6, 1),
            RegisterBitDefinition("PGS_ERR", 7, 1),
            RegisterBitDefinition("MISS_ERR", 8, 1),
            RegisterBitDefinition("FAST_ERR", 9, 1),
            RegisterBitDefinition("_", 10, 3),
            RegisterBitDefinition("OPTNV", 13, 1),
            RegisterBitDefinition("RD_ERR", 14, 1),
            RegisterBitDefinition("OPTV_ERR", 15, 1),
            RegisterBitDefinition("BSY", 16, 1),
            RegisterBitDefinition("_", 17, 1),
            RegisterBitDefinition("CFGBSY", 18, 1),
            RegisterBitDefinition("PESD", 19, 1),
            RegisterBitDefinition("_", 20, 12),
        ],
    )

    # Option byte registers
    FLASH_OPTR = FLASH_BASE + 0x20
    FLASH_PCROP1ASR = FLASH_BASE + 0x24
    FLASH_PCROP1AER = FLASH_BASE + 0x28
    FLASH_WRP1AR = FLASH_BASE + 0x2C
    FLASH_WRP1BR = FLASH_BASE + 0x30
    FLASH_PCROP1BSR = FLASH_BASE + 0x34
    FLASH_PCROP1BER = FLASH_BASE + 0x38
    FLASH_IPCCBR = FLASH_BASE + 0x3C

    # Map option byte dword index to register address
    OPTION_BYTE_MAP_TO_REGS = {
        0: FLASH_OPTR,
        1: FLASH_PCROP1ASR,
        2: FLASH_PCROP1AER,
        3: FLASH_WRP1AR,
        4: FLASH_WRP1BR,
        5: FLASH_PCROP1BSR,
        6: FLASH_PCROP1BER,
        7: None,  # Invalid Options
        8: None,  # Invalid Options
        9: None,  # Invalid Options
        10: None,  # Invalid Options
        11: None,  # Invalid Options
        12: None,  # Invalid Options
        13: FLASH_IPCCBR,
        14: None,  # Secure Flash
        15: None,  # Core 2 Options
    }


class Main(App):
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

    def _ob_print_diff_table(self, ob_reference: bytes, ob_read: bytes, print_fn):
        print_fn(
            f'{"Reference": <20} {"Device": <20} {"Diff Reference": <20} {"Diff Device": <20}'
        )

        diff1 = bytes()
        diff2 = bytes()

        # Split into 8 byte, word + word
        for i in range(0, len(ob_reference), 8):
            ref = ob_reference[i : i + 8]
            read = ob_read[i : i + 8]

            diff_str1 = ""
            diff_str2 = ""
            for j in range(0, len(ref.hex()), 2):
                byte_str_1 = ref.hex()[j : j + 2]
                byte_str_2 = read.hex()[j : j + 2]

                if byte_str_1 == byte_str_2:
                    diff_str1 += "__"
                    diff_str2 += "__"
                else:
                    diff_str1 += byte_str_1
                    diff_str2 += byte_str_2

            print_fn(
                f"{ref.hex(): <20} {read.hex(): <20} {diff_str1: <20} {diff_str2: <20}"
            )

    def check(self):
        self.logger.info(f"Checking Option Bytes")

        # OpenOCD
        oocd = OpenOCD(self._get_dap_params())
        oocd.start()
        oocd.send_tcl("reset init")

        # Registers
        stm32 = STM32WB55()

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
            addr = stm32.OPTION_BYTE_BASE + i * 4
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
            self._ob_print_diff_table(ob_reference, ob_compare, self.logger.error)
            return_code = 1

        # Stop OpenOCD
        oocd.send_tcl("reset run")
        oocd.stop()

        return return_code

    def flash_unlock(self, oocd: OpenOCD, stm32: STM32WB55):
        # Check if flash is already unlocked
        stm32.FLASH_CR.load(oocd)
        if stm32.FLASH_CR.LOCK == 0:
            self.logger.debug("Flash is already unlocked")
            return

        # Unlock flash
        self.logger.debug("Unlocking Flash")
        oocd.write_32(stm32.FLASH_KEYR, stm32.FLASH_UNLOCK_KEY1)
        oocd.write_32(stm32.FLASH_KEYR, stm32.FLASH_UNLOCK_KEY2)

        # Check if flash is unlocked
        stm32.FLASH_CR.load(oocd)
        if stm32.FLASH_CR.LOCK == 0:
            self.logger.debug("Flash unlocked")
        else:
            self.logger.error("Flash unlock failed")
            raise Exception("Flash unlock failed")

    def option_bytes_unlock(self, oocd: OpenOCD, stm32: STM32WB55):
        # Check if options is already unlocked
        stm32.FLASH_CR.load(oocd)
        if stm32.FLASH_CR.OPT_LOCK == 0:
            self.logger.debug("Options is already unlocked")
            return

        # Unlock options
        self.logger.debug("Unlocking Options")
        oocd.write_32(stm32.FLASH_OPTKEYR, stm32.FLASH_UNLOCK_OPTKEY1)
        oocd.write_32(stm32.FLASH_OPTKEYR, stm32.FLASH_UNLOCK_OPTKEY2)

        # Check if options is unlocked
        stm32.FLASH_CR.load(oocd)
        if stm32.FLASH_CR.OPT_LOCK == 0:
            self.logger.debug("Options unlocked")
        else:
            self.logger.error("Options unlock failed")
            raise Exception("Options unlock failed")

    def option_bytes_lock(self, oocd: OpenOCD, stm32: STM32WB55):
        # Check if options is already locked
        stm32.FLASH_CR.load(oocd)
        if stm32.FLASH_CR.OPT_LOCK == 1:
            self.logger.debug("Options is already locked")
            return

        # Lock options
        self.logger.debug("Locking Options")
        stm32.FLASH_CR.OPT_LOCK = 1
        stm32.FLASH_CR.store(oocd)

        # Check if options is locked
        stm32.FLASH_CR.load(oocd)
        if stm32.FLASH_CR.OPT_LOCK == 1:
            self.logger.debug("Options locked")
        else:
            self.logger.error("Options lock failed")
            raise Exception("Options lock failed")

    def flash_lock(self, oocd: OpenOCD, stm32: STM32WB55):
        # Check if flash is already locked
        stm32.FLASH_CR.load(oocd)
        if stm32.FLASH_CR.LOCK == 1:
            self.logger.debug("Flash is already locked")
            return

        # Lock flash
        self.logger.debug("Locking Flash")
        stm32.FLASH_CR.LOCK = 1
        stm32.FLASH_CR.store(oocd)

        # Check if flash is locked
        stm32.FLASH_CR.load(oocd)
        if stm32.FLASH_CR.LOCK == 1:
            self.logger.debug("Flash locked")
        else:
            self.logger.error("Flash lock failed")
            raise Exception("Flash lock failed")

    def clear_flash_errors(self, oocd: OpenOCD, stm32: STM32WB55):
        # Errata 2.2.9: Flash OPTVERR flag is always set after system reset
        # And also clear all other flash error flags
        self.logger.debug(f"Errata 2.2.9 workaround")
        stm32.FLASH_SR.load(oocd)
        stm32.FLASH_SR.OP_ERR = 1
        stm32.FLASH_SR.PROG_ERR = 1
        stm32.FLASH_SR.WRP_ERR = 1
        stm32.FLASH_SR.PGA_ERR = 1
        stm32.FLASH_SR.SIZE_ERR = 1
        stm32.FLASH_SR.PGS_ERR = 1
        stm32.FLASH_SR.MISS_ERR = 1
        stm32.FLASH_SR.FAST_ERR = 1
        stm32.FLASH_SR.RD_ERR = 1
        stm32.FLASH_SR.OPTV_ERR = 1
        stm32.FLASH_SR.store(oocd)

    def _unpack_u32(self, data: bytes, offset: int):
        return int.from_bytes(data[offset : offset + 4], "little")

    def set(self):
        self.logger.info(f"Setting Option Bytes")

        # OpenOCD
        oocd = OpenOCD(self._get_dap_params())
        oocd.start()
        oocd.send_tcl("reset init")

        # Registers
        stm32 = STM32WB55()

        # Generate Option Bytes data
        ob_data = OptionBytesData(self.args.ob_path)
        ob_values = ob_data.gen_values().export()
        ob_reference_bytes = ob_values.reference
        ob_compare_mask_bytes = ob_values.compare_mask
        ob_write_mask_bytes = ob_values.write_mask
        ob_length = len(ob_reference_bytes)
        ob_dwords = int(ob_length / 8)

        # Clear flash errors
        self.clear_flash_errors(oocd, stm32)

        # Unlock Flash and Option Bytes
        self.flash_unlock(oocd, stm32)
        self.option_bytes_unlock(oocd, stm32)

        ob_need_to_apply = False

        for i in range(ob_dwords):
            device_addr = stm32.OPTION_BYTE_BASE + i * 8
            device_value = oocd.read_32(device_addr)
            ob_write_mask = self._unpack_u32(ob_write_mask_bytes, i * 8)
            ob_compare_mask = self._unpack_u32(ob_compare_mask_bytes, i * 8)
            ob_value_ref = self._unpack_u32(ob_reference_bytes, i * 8)
            ob_value_masked = device_value & ob_compare_mask

            need_patch = ((ob_value_masked ^ ob_value_ref) & ob_write_mask) != 0
            if need_patch:
                ob_need_to_apply = True
                device_register_index = int(i / 1)

                self.logger.info(
                    f"Need to patch: {device_addr:08X}: {ob_value_masked:08X} != {ob_value_ref:08X}, REG[{device_register_index}]"
                )

                # Check if this option byte (dword) is mapped to a register
                device_reg_addr = stm32.OPTION_BYTE_MAP_TO_REGS.get(
                    device_register_index, None
                )
                if device_reg_addr is None:
                    # self.logger.error(f"Option Byte {i} is not mapped to a register")
                    # continue
                    raise Exception(
                        f"Option Byte {device_register_index} is not mapped to a register"
                    )

                # Construct new value for the OB register
                ob_value = device_value & (~ob_write_mask)
                ob_value |= ob_value_ref & ob_write_mask

                self.logger.info(f"Writing {ob_value:08X} to {device_reg_addr:08X}")
                oocd.write_32(device_reg_addr, ob_value)

        if ob_need_to_apply:
            self.logger.debug(f"Applying Option Bytes")

            stm32.FLASH_CR.load(oocd)
            stm32.FLASH_CR.OPT_STRT = 1
            stm32.FLASH_CR.store(oocd)

            # Wait for Option Bytes to be applied
            # TODO: timeout
            while True:
                stm32.FLASH_SR.load(oocd)
                if stm32.FLASH_SR.BSY == 0:
                    break
        else:
            self.logger.info(f"Option Bytes are already correct")

        # Load Option Bytes
        # That will reset and also lock the Option Bytes and the Flash
        stm32.FLASH_CR.load(oocd)
        stm32.FLASH_CR.OBL_LAUNCH = 1
        stm32.FLASH_CR.store(oocd)

        # Stop OpenOCD
        oocd.send_tcl("reset run")
        oocd.stop()

        return 0


if __name__ == "__main__":
    Main()()
