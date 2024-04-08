from typing import List


class command_result:
    @property
    def Command(self):
        return self.__Command

    @property
    def Result(self):
        return self.__Result

    def __init__(self, input):
        print("input: ", input)
        s = str(input).split("\n")
        # remove empty lines
        s = list(filter(None, s))
        self.__Command = s[0]
        self.__Result = s[1:]


class proxmark_command:
    @property
    def Command(self):
        return self.__Command

    @property
    def Result(self):
        return self.__Result

    @property
    def Expected(self):
        return self.__Expected

    def parse_result(self, result):
        s = str(input).split("\n")
        self.__Result = command_result(result)

    def __init__(self, cmd, expected):
        self.__Command = cmd
        self.__Expected = expected


class Scenario:
    def __init__(self, json):
        self.Name = self.__decode(json, "Name")
        self.Card = self.__decode(json, "Card")
        self.Description = self.__decode(json, "Description")
        self.Cmds = []
        for jsc in json["Cmds"]:
            cmd = proxmark_command(jsc["cmd"], jsc["result"])
            self.Cmds.append(cmd)

    def __decode(self, json, name):
        try:
            return json[name]
        except KeyError:
            return ""

    def get_commands(self) -> List[str]:
        result = []
        for cmd in self.Cmds:
            result.append(cmd.Command + "\n")
        return result
