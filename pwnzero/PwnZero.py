import serial
from enum import Enum

class PwnZeroParam(Enum):
    """
    Flipper Zero Parameters
    These are the parameters that can be changed on the Flipper Zero
    The values are the bytes that are being sent to the Flipper Zero
    to change the parameter
    The documentation for the Flipper Zero can be found here:
    https://github.com/Matt-London/pwnagotchi-flipper/blob/main/doc/Protocol.md
    """
    FACE        = 4
    NAME        = 5
    CHANNEL     = 6
    APS         = 7
    UPTIME      = 8
    FRIEND      = 9
    MODE        = 10
    HANDSHAKES  = 11
    MESSAGE     = 12

    class Mode(Enum):
        """
        Embedded class with the mode
        """
        MANU    = 4
        AUTO    = 5
        AI      = 6

    class Face(Enum):
        """
        Embedded class with all face parameters
        """
        NO_FACE         = 4
        DEFAULT_FACE    = 5
        LOOK_R          = 6
        LOOK_L          = 7
        LOOK_R_HAPPY    = 8
        LOOK_L_HAPPY    = 9
        SLEEP           = 10
        SLEEP2          = 11
        AWAKE           = 12
        BORED           = 13
        INTENSE         = 14
        COOL            = 15
        HAPPY           = 16
        GRATFUL         = 17
        EXCITED         = 18
        MOTIVATED       = 19
        DEMOTIVATED     = 20
        SMART           = 21
        LONELY          = 22
        SAD             = 23
        ANGRY           = 24
        FRIEND          = 25
        BROKEN          = 26
        DEBUG           = 27
        UPLOAD          = 28
        UPLOAD1         = 29
        UPLOAD2         = 30


class PwnZero(object):
    PROTOCOL_START   = 0x02
    PROTOCOL_END     = 0x03

    def __init__(self, port: str, baud: int = 115200):
        """
        Construct a PwnZero object, this will create the connection

        :param: port: Port on which the UART of the Flipper is connected to
        :param: baud: Baudrate for communication to the Flipper (default 115200)
        """
        self._port = port
        self._baud = baud

        try:
            self._serialConn = serial.Serial(port, baud)
        except:
            raise "Cannot bind to port ({}) with baud ({})".format(port, baud)

    def _is_byte(self, i: int) -> bool:
        """
        Checks if a passed in integer is a valid byte (0 <= i < 256)

        :param: i: Integer to check
        :return: If it is a valid byte
        """
        return 0 <= i < 256

    def _str_to_bytes(self, s: str) -> list[int]:
        """
        Converts a string into a list of bytes

        :param: s: String to convert
        :return: List of bytes
        """
        retVal = []
        for c in s:
            retVal.append(ord(c))
        
        return retVal


    def _send_data(self, param: int, args: list[int]) -> bool:
        """
        Sends data using protocol v2 over the serial port to the Flipper Zero

        :param: param: Parameter that is being changed
        :param: args: Arguments to pass to the flipper
        :return: If transmission was successful
        """
        # Make sure everything is a valid byte
        if not self._is_byte(param):
            return False
        for i in args:
            if not self._is_byte(i):
                return False
        
        # Now we know everything is a valid byte
        
        # Build the sending data
        data = [self.PROTOCOL_START]
        data.append(param)
        for arg in args:
            data.append(arg)
        
        data.append(self.PROTOCOL_END)

        # Send data to flipper
        return self._serialConn.write(data) == len(data)


