#!/usr/bin/env python3

from flipper.app import App

import logging
import os
import time
import subprocess


class Main(App):
    def init(self):
        self.parser.add_argument("-p", "--port", help="CDC Port", default="auto")
        self.parser.set_defaults(func=self.update)

        # logging
        self.logger = logging.getLogger()

    def lookup(self):
        import serial.tools.list_ports as list_ports

        ports = list(list_ports.grep("ESP32-S2"))

        if len(ports) == 0:
            # Blackmagic probe serial port not found, will be handled later
            pass
        elif len(ports) > 2:
            raise Exception("More than one Blackmagic board found")
        else:
            port = ports[0]
            if os.name == "nt":
                port.device = f"\\\\.\\{port.device}"
            return port.device

    def download_latest(self, dir):
        import requests

        # TODO: get latest version
        urls = [
            "https://update.flipperzero.one/builds/blackmagic-firmware/zlo/dap-link/flash.command",
            "https://update.flipperzero.one/builds/blackmagic-firmware/zlo/dap-link/blackmagic.bin",
            "https://update.flipperzero.one/builds/blackmagic-firmware/zlo/dap-link/bootloader.bin",
            "https://update.flipperzero.one/builds/blackmagic-firmware/zlo/dap-link/partition-table.bin",
        ]

        if not os.path.exists(dir):
            self.logger.info(f"Creating directory {dir}")
            os.makedirs(dir)

        for url in urls:
            file_name = url.split("/")[-1]
            file_path = os.path.join(dir, file_name)
            self.logger.info(f"Downloading {url} to {file_path}")
            with open(file_path, "wb") as f:
                response = requests.get(url)
                f.write(response.content)

    def update(self):
        # TODO: get real temporary dir
        dir = "./tmp"

        port = self.lookup()

        if port is None:
            self.logger.error("Blackmagic probe not found")
            return 1

        self.download_latest(dir)

        with open(os.path.join(dir, "flash.command"), "r") as f:
            flash_command = f.read()

        flash_command = flash_command.replace("\n", "").replace("\r", "")
        flash_command = flash_command.replace("(PORT)", port)

        args = flash_command.split(" ")[0:]
        args = list(filter(None, args))

        esptool_params = []
        esptool_params.extend(args)

        self.logger.info(f'Running command: "{" ".join(args)}"')

        process = subprocess.Popen(
            esptool_params,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            cwd=dir,
        )

        while process.poll() is None:
            time.sleep(0.25)
            print(".", end="", flush=True)
        print()

        if process.stdout:
            self.logger.info(process.stdout.read().decode("utf-8").strip())

        return process.returncode


if __name__ == "__main__":
    Main()()
