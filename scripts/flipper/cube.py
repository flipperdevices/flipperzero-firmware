import logging
import subprocess

class CubeProgrammer:
    """STM32 Cube Programmer cli wrapper"""

    def __init__(self, port, params=[]):
        self.port = port
        self.params = params
        # logging
        self.logger = logging.getLogger()

    def _execute(self, args):
        try:
            output = subprocess.check_output(
                [
                    "STM32_Programmer_CLI",
                    "-q",
                    f"-c port={self.port}",
                    *self.params,
                    *args,
                ]
            )
        except subprocess.CalledProcessError as e:
            print(e.output)
            raise e
        assert(output)
        return output.decode()


    def getVersion(self):
        output = self._execute(["--version"])

    def checkOptionBytes(self, option_bytes):
        output = self._execute(["-ob displ"])
        ob_correct = True
        for line in output.split("\n"):
            line = line.strip()
            if not ":" in line:
                self.logger.debug(f"Skipping line: {line}")
                continue
            key, data = line.split(":", 1)
            key = key.strip()
            data = data.strip()
            if not key in option_bytes.keys():
                self.logger.debug(f"Skipping key: {key}")
                continue
            self.logger.debug(f"Processing key: {key} {data}")
            value, comment = data.split(" ", 1)
            value = value.strip()
            comment = comment.strip()
            if option_bytes[key][0] != value:
                self.logger.error(
                    f"Invalid OB: {key} {value}, expected: {option_bytes[key][0]}"
                )
                ob_correct = False
        return ob_correct

    def setOptionBytes(self, option_bytes):
        options = []
        for key, (value, attr) in option_bytes.items():
            if "w" in attr:
                options.append(f"{key}={value}")
        self._execute([ "-ob", *options ])
        return True