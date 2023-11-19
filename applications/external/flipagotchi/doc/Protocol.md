# Stage 1 Communication Protocol (v2.0.1)
This document will detail the protocol used to communicate between the flipper and the pwnagotchi.

## Protocol Requirements
What actually needs to be communicable through this protocol?

### Pwnagotchi --> Flipper
- Current device state
  - Is pwnagotchi service running?
- Current screen state
  - Face
  - Name
  - Channel
  - Aps
  - Uptime
  - Friend
  - Mode
  - Handshakes
  - Message

### Pwnagotchi <-- Flipper
- Query device state
  - Can we actually communicate with the connection?

### Transmission size
- Due to some of the parameters being variable length (like the message) a size of transmission cannot be enforced. For that reason a start and end byte will be used to decide when the transmission is complete
- Each message code will handle some form of error checking in communication and ensure that for length-set parameters like Face (integer) a specific transmission size is enforced

## Protocol Specification
This protocol is focused on being able to transmit to the Flipper a parameter to change followed by the new value of that parameter. Each time it is updated on the Pwnagotchi's end, the Pwnagotchi will send all new parameters to the Flipper and update the screen. Stage 2 will be more complicated and support sending and receiving other data to allow interaction.

**Transmission:**
| Byte # | Content | Hex  | Description                 |
| ------ | ------- | ---- | --------------------------- |
| 1      | STX     | 0x02 | Start byte                  |
| 2      | Param   |      | Parameter to change         |
| 3..N   | Args    |      | Arguments for the parameter |
| N+1    | ETX     | 0x03 | End byte                    |

**Parameter codes:**
| Code | Parameter  |
| ---- | ---------- |
| 4    | Face       |
| 5    | Name       |
| 6    | Channel    |
| 7    | APS        |
| 8    | Uptime     |
| 9    | Friend     |
| 10   | Mode       |
| 11   | Handshakes |
| 12   | Message    |

## Protocol Usage
This section will explain the usage of each parameter and they're associated arguments.

### Face:
To change the display's face a message with the following structure should be sent:
```
0x02 0x04 [Face_code] 0x03
```
Replace ```[Face_code]``` with the following code according to the face you need to set.
| Code | Face         | Face String |
| ---- | ------------ | ----------- |
| 4    | NoFace       |             |
| 5    | DefaultFace  | *See Awake* |
| 6    | Look_r       |   ( ⚆_⚆)   |
| 7    | Look_l       |   (☉_☉ )   |
| 8    | Look_r_happy |   ( ◕‿◕)  |
| 9    | Look_l_happy |   (◕‿◕ )  |
| 10   | Sleep        |   (⇀‿‿↼) |
| 11    | Sleep2       |   (≖‿‿≖)  |
| 12    | Awake        |   (◕‿‿◕)  |
| 13    | Bored        |   (-__-)    |
| 14   | Intense      |   (°▃▃°)   |
| 15   | Cool         |   (⌐■_■)    |
| 16   | Happy        |   (•‿‿•)   |
| 17   | Grateful     |   (^‿‿^)   |
| 18   | Excited      |   (ᵔ◡◡ᵔ)   |
| 19   | Motivated    |   (☼‿‿☼)   |
| 20   | Demotivated  |   (≖__≖)    |
| 21   | Smart        |   (✜‿‿✜)  |
| 22   | Lonely       |   (ب__ب)    |
| 23   | Sad          |   (╥☁╥ )   |
| 24   | Angry        |   (-_-')    |
| 25   | Friend       |   (♥‿‿♥)   |
| 26   | Broken       |   (☓‿‿☓)  |
| 27   | Debug        |   (#__#)    |
| 28   | Upload       |   (1__0)    |
| 29   | Upload1      |   (1__1)    |
| 30   | Upload2      |   (0__1)    |

Setting the face to Smart looks like this:
```
0x02 0x04 0x15 0x03
```

### Name:
Changing the hostname of the Pwnagotchi:
```
0x02 0x05 [ASCII_char_1] [ASCII_char_2]..[ASCII_char_N] 0x03
```

Setting the hostname to "Pwn":
```
//         "P"  "w"  "n"
0x02 0x05 0x50 0x77 0x6e 0x03
```

### Channel:
The channel can be any number between 1 and 255 (6 GHz supports up to 233) or '*' for all channels.
Because sending a number as 2 or 3 will confuse the protocol you will send this as ascii as well.

Setting channel:
```
0x02 0x06 [ASCII_char_1]...[ASCII_char_3] 0x03
```

Setting channel to 11:
```
//         "1"  "1"
0x02 0x06 0x31 0x32 0x03
```

### APS (Access Points):
The APS section of the Pwnagotchi access points is displayed as ```[APS_this_session] ([total_APS])```
For this reason the APS should be transmitted as ASCII characters. Including ASCII for the numbers, space, and parentheses
Setting APS:
```
0x02 0x07 [ASCII_char_1] [ASCII_char_2]..[ASCII_char_N] 0x03
```

Setting APS to 5 (10)
```
//         "5"  " "  "(" "1"   "0"  ")"
0x02 0x07 0x35 0x20 0x28 0x31 0x30 0x29 0x03
```

### Uptime:
Uptime is displayed on the Pwnagotchi as ```hh:mm:ss```
Just like above this will be transmitted as ASCII

Setting uptime:
```
//                                       ":"                                ":"
0x02 0x08 [ASCII_char_h] [ASCII_char_h] 0x3a [ASCII_char_m] [ASCII_char_m] 0x3a [ASCII_char_s] [ASCII_char_s] 0x03
```

Setting uptime to "05:12:36"
```
//         "0"  "5" ":"  "1"  "2"  ":"  "3"   "6"
0x02 0x08 0x30 0x35 0x3a 0x31 0x32 0x3a 0x33 0x36 0x03
```

### Friend:
This parameter is not yet implemented.

### Mode:
This parameter sets the display Pwnagotchi mode between MANU, AUTO, and AI

Setting mode:
```
0x02 0x0a [mode_code] 0x03
```
| Code | Mode |
| ---- | ---- |
| 4    | MANU |
| 5    | AUTO |
| 6    | AI   |

Setting mode to AI:
```
//         AI
0x02 0x0a 0x06 0x03
```

### Handshakes:
This parameter is displayed as PWND on the Pwnagotchi.
It has a similar format to APS in that: ```[shakes_this_session] ([total_shakes])```
It will also be transmitted as ASCII like in APS

Setting handshakes:
```
0x02 0x0b [ASCII_char_1] [ASCII_char_2]..[ASCII_char_N] 0x03
```

Setting handshakes to "3 (12)":
```
//         "3"  " "  "(" "1"   "2"  ")"
0x02 0x0b 0x33 0x20 0x28 0x31 0x32 0x29 0x03
```

### Message:
This parameter is displayed underneath the hostname of the Pwnagotchi and confers the current
status of the program. It is a variable length ASCII message.

Setting message:
```
0x02 0x0c [ASCII_char_1] [ASCII_char_2]..[ASCII_char_N] 0x03
```

Setting the message to "Hack the planet!":
```
//         "H" "a"  "c"  "k"  " "  "t"  "h"  "e"  " "  "p"  "l"  "a"  "n"  "e"  "t"  "!"
0x02 0x0c 0x48 0x61 0x63 0x6b 0x20 0x74 0x68 0x65 0x20 0x70 0x6c 0x61 0x6e 0x65 0x74 0x21 0x03
```
