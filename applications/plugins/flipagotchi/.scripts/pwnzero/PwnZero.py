import serial
from enum import Enum

import pwnagotchi.plugins as plugins
import pwnagotchi.ui.faces as faces

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

class PwnMode(Enum):
        """
        Embedded class with the mode
        """
        MANU    = 4
        AUTO    = 5
        AI      = 6

class PwnFace(Enum):
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
    GRATEFUL         = 17
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


class PwnZero(plugins.Plugin):
    __author__ = "github.com/Matt-London"
    __version__ = "1.0.0"
    __license__ = "MIT"
    __description__ = "Plugin to display the Pwnagotchi on the Flipper Zero"

    PROTOCOL_START   = 0x02
    PROTOCOL_END     = 0x03

    def __init__(self, port: str = "/dev/serial0", baud: int = 115200):
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

    def close(self):
        """
        Closes the connection to the Flipper Zero
        """
        self._serialConn.close()

    def _is_byte(self, i: int) -> bool:
        """
        Checks if a passed in integer is a valid byte (0 <= i < 256)

        :param: i: Integer to check
        :return: If it is a valid byte
        """
        return 0 <= i < 256

    def _str_to_bytes(self, s: str):
        """
        Converts a string into a list of bytes

        :param: s: String to convert
        :return: List of bytes
        """
        retVal = []
        for c in s:
            retVal.append(ord(c))
        
        return retVal

    def _send_data(self, param: int, args) -> bool:
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

    # Public method commands
    def set_face(self, face: PwnFace) -> bool:
        """
        Set the face of the Pwnagotchi

        :param: face: Face to set on the device
        :return: If the command was sent successfully
        """
        return self._send_data(PwnZeroParam.FACE.value, [face.value])

    def set_name(self, name: str) -> bool:
        """
        Set the name of the Pwnagotchi

        :param: name: Name to set on the pwnagotchi
        :return: If the command was sent successfully
        """
        data = self._str_to_bytes(name)
        return self._send_data(PwnZeroParam.NAME.value, data)

    def set_channel(self, channel: int) -> bool:
        """
        Set the channel of the Pwnagotchi
        Send a 0 for * (all channels)

        :param: channel: Channel to set on pwnagotchi
        :return: If the command was sent successfully
        """
        # Make sure channel is valid
        if not (0 <= channel <= 255):
            return False
        
        channelStr = "*"

        if channel != 0:
            channelStr = str(channel)

        data = self._str_to_bytes(channelStr)
        return self._send_data(PwnZeroParam.CHANNEL.value, data)

    def set_aps(self, apsCurrent: int, apsTotal) -> bool:
        """
        Set the APs of the Pwnagotchi

        :param: apsCurrent: Number of APS this session
        :param: apsTotal: Number of APS in unit lifetime
        :return: If the command was sent successfully
        """
        data = self._str_to_bytes("{} ({})".format(apsCurrent, apsTotal))
        return self._send_data(PwnZeroParam.APS.value, data)

    def set_uptime(self, hh: int, mm: int, ss: int) -> bool:
        """
        Sets the uptime of the Pwnagotchi

        :param: hh: Hours
        :param: mm: Minutes
        :param: ss: Seconds
        :return: If the command was sent successfully
        """
        # Make sure all values are less than 100 and greater than 0
        if not (0 <= hh < 100 and 0 <= mm < 100 and 0 <= ss < 100):
            return False

        # A stands for adjusted
        hhA = str(hh).zfill(2)
        mmA = str(mm).zfill(2)
        ssA = str(ss).zfill(2)

        data = self._str_to_bytes("{}:{}:{}".format(hhA, mmA, ssA))
        return self._send_data(PwnZeroParam.UPTIME.value, data)

    def set_friend(self) -> bool:
        """
        Friend is currently not supported
        
        :return: False
        """
        return False

    def set_mode(self, mode: PwnMode) -> bool:
        """
        Set the mode on the Pwnagotchi
        
        :param: mode: Mode to set
        :return: If the command was sent successfully
        """
        return self._send_data(PwnZeroParam.MODE.value, [mode.value])

    def set_handshakes(self, handshakesCurrent: int, handshakesTotal: int) -> bool:
        """
        Set the number of handshakes on the Pwnagotchi

        :param: handshakesCurrent: Number of handshakes this session
        :param: handshakesTotal: Number of handshakes in the lifetime of unit
        :return: If the command was sent successfully
        """
        data = self._str_to_bytes("{} ({})".format(handshakesCurrent, handshakesTotal))
        return self._send_data(PwnZeroParam.HANDSHAKES.value, data)

    def set_message(self, message: str) -> bool:
        """
        Sets the displayed message on the Pwnagotchi
        
        :param: message: Message to set
        :return: If the command was sent successfully
        """
        data = self._str_to_bytes(message)
        return self._send_data(PwnZeroParam.MESSAGE.value, data)

    def on_ui_setup(self, ui):
        pass

    def on_ui_update(self, ui):
        # Message
        self.set_message(ui.get('status'))

        # Mode
        modeEnum = None
        if ui.get('mode') == 'AI':
            modeEnum = PwnMode.AI
        elif ui.get('mode') == 'MANU':
            modeEnum = PwnMode.MANU
        elif ui.get('mode') == 'AUTO':
            modeEnum = PwnMode.AUTO
        self.set_mode(modeEnum)

        # Channel
        channelInt = 0
        channel = ui.get('channel')
        if channel == '*':
            channelInt = 0
        else:
            channelInt = int(channel)
        self.set_channel(channelInt)

        # Uptime
        uptime = ui.get('uptime')
        uptimeSplit = uptime.split(':')
        self.set_uptime(int(uptimeSplit[0]), int(uptimeSplit[1]), int(uptimeSplit[2]))

        # APS
        aps = ui.get('aps')
        
        # name
        self.set_name(ui.get('name').replace(">", ""))

        # Face
        face = ui.get('face')
        
        faceEnum = None
        if face == faces.LOOK_R:
            faceEnum = PwnFace.LOOK_R
        elif face == faces.LOOK_L:
            faceEnum = PwnFace.LOOK_L
        elif face == faces.LOOK_R_HAPPY:
            faceEnum = PwnFace.LOOK_R_HAPPY
        elif face == faces.LOOK_L_HAPPY:
            faceEnum = PwnFace.LOOK_L_HAPPY
        elif face == faces.SLEEP:
            faceEnum = PwnFace.SLEEP
        elif face == faces.SLEEP2:
            faceEnum = PwnFace.SLEEP2
        elif face == faces.AWAKE:
            faceEnum = PwnFace.AWAKE
        elif face == faces.BORED:
            faceEnum = PwnFace.BORED
        elif face == faces.INTENSE:
            faceEnum = PwnFace.INTENSE
        elif face == faces.COOL:
            faceEnum = PwnFace.COOL
        elif face == faces.HAPPY:
            faceEnum = PwnFace.HAPPY
        elif face == faces.GRATEFUL:
            faceEnum = PwnFace.GRATEFUL
        elif face == faces.EXCITED:
            faceEnum = PwnFace.EXCITED
        elif face == faces.MOTIVATED:
            faceEnum = PwnFace.MOTIVATED
        elif face == faces.DEMOTIVATED:
            faceEnum = PwnFace.DEMOTIVATED
        elif face == faces.SMART:
            faceEnum = PwnFace.SMART
        elif face == faces.LONELY:
            faceEnum = PwnFace.LONELY
        elif face == faces.SAD:
            faceEnum = PwnFace.SAD
        elif face == faces.ANGRY:
            faceEnum = PwnFace.ANGRY
        elif face == faces.FRIEND:
            faceEnum = PwnFace.FRIEND
        elif face == faces.BROKEN:
            faceEnum = PwnFace.BROKEN
        elif face == faces.DEBUG:
            faceEnum = PwnFace.DEBUG
        elif face == faces.UPLOAD:
            faceEnum = PwnFace.UPLOAD
        elif face == faces.UPLOAD1:
            faceEnum = PwnFace.UPLOAD1
        elif face == faces.UPLOAD2:
            faceEnum = PwnFace.UPLOAD2

        self.set_face(faceEnum)

        # Handshakes
        handshakes = ui.get('shakes')

        shakesCurr = handshakes.split(' ')[0]
        shakesTotal = handshakes.split(' ')[1].replace(')', '').replace('(', '')
