import logging
import subprocess
import time
from typing import List
from .command_result import command_result


class proxmark_wrapper:
    @property
    def PM3(self):
        return self.__PM3

    def __init__(self, path):
        self.__PM3 = path
        self.logger = logging.getLogger("proxmark_wrapper")

    def execute_cmds(self, cmds) -> List[command_result]:
        with subprocess.Popen(
            [self.__PM3],
            text=True,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            encoding="cp866",
        ) as process:
            for cmd in cmds:
                process.stdin.write(cmd)

            process.stdin.write("exit\n")
            process.stdin.close()
            data_out = process.stdout.read()

        result = self.__get_result(data_out)
        return result

    def execute(self, cmd) -> List[command_result]:
        with subprocess.Popen(
            [self.__PM3],
            text=True,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            encoding=("cp866"),
        ) as process:
            process.stdin.write(cmd + "\n")
            process.stdin.write("exit\n")
            process.stdin.close()
            data_out = process.stdout.read()

        result = self.__get_result(data_out)
        return result[0]

    def shutdown(self):
        pass

    def __get_result(self, data):
        cmd_result = self.__parse_data(data)
        return cmd_result

    def __parse_data(self, output_data):
        raw_result = str(output_data).split("[usb|script] pm3 --> ")

        self.logger.debug("raw_result: %s", raw_result)
        self.logger.debug("-------------------------------------")

        result = []
        for raw in raw_result[1:]:
            result.append(command_result(raw).Result)
        return result
