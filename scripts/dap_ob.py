#!/usr/bin/env python3
from flipper.app import App
from flipper.assets.obdata import OptionBytesData
from flipper.utils.openocd import OpenOCD
from flipper.utils.stm32wb55 import STM32WB55


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

        # Registers
        stm32 = STM32WB55()

        # OpenOCD
        oocd = OpenOCD(self._get_dap_params())
        oocd.start()
        stm32.reset(oocd, stm32.RunMode.Init)

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
        stm32.reset(oocd, stm32.RunMode.Run)
        oocd.stop()

        return return_code

    def _unpack_u32(self, data: bytes, offset: int):
        return int.from_bytes(data[offset : offset + 4], "little")

    def set(self):
        self.logger.info(f"Setting Option Bytes")

        # Registers
        stm32 = STM32WB55()

        # OpenOCD
        oocd = OpenOCD(self._get_dap_params())
        oocd.start()
        stm32.reset(oocd, stm32.RunMode.Init)

        # Generate Option Bytes data
        ob_data = OptionBytesData(self.args.ob_path)
        ob_values = ob_data.gen_values().export()
        ob_reference_bytes = ob_values.reference
        ob_compare_mask_bytes = ob_values.compare_mask
        ob_write_mask_bytes = ob_values.write_mask
        ob_length = len(ob_reference_bytes)
        ob_dwords = int(ob_length / 8)

        # Clear flash errors
        stm32.clear_flash_errors(oocd)

        # Unlock Flash and Option Bytes
        stm32.flash_unlock(oocd)
        stm32.option_bytes_unlock(oocd)

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

                self.logger.info(
                    f"Need to patch: {device_addr:08X}: {ob_value_masked:08X} != {ob_value_ref:08X}, REG[{i}]"
                )

                # Check if this option byte (dword) is mapped to a register
                device_reg_addr = stm32.option_bytes_id_to_address(i)

                # Construct new value for the OB register
                ob_value = device_value & (~ob_write_mask)
                ob_value |= ob_value_ref & ob_write_mask

                self.logger.info(f"Writing {ob_value:08X} to {device_reg_addr:08X}")
                oocd.write_32(device_reg_addr, ob_value)

        if ob_need_to_apply:
            stm32.option_bytes_apply(oocd)
        else:
            self.logger.info(f"Option Bytes are already correct")

        # Load Option Bytes
        # That will reset and also lock the Option Bytes and the Flash
        stm32.option_bytes_load(oocd)

        # Stop OpenOCD
        stm32.reset(oocd, stm32.RunMode.Run)
        oocd.stop()

        return 0


if __name__ == "__main__":
    Main()()
