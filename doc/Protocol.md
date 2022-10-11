# Stage 1 Communication Protocol (v1.1.0)
This document will detail the protocol used to communicate between the flipper and the pwnagotchi.

## Protocol Requirements
What actually needs to be communicable through this protocol?

### Pwnagotchi --> Flipper
- Current screen state
  - Setting up a protocol to tell the flipper what pixels should be turned on
- Current device state
  - Is pwnagotchi service running?

### Flipper --> Pwnagotchi
- Query device state
  - Can we actually communicate with the connection?

### Transmission size
- The Flipper has screen dimensions 128px by 64px
  - This means that to request a coordinate be turned on or off we need to represent numbers with a max of 127 by 63
- To represent these simply we will use five total bytes: Start, i coord, j coord, status, end

## Protocol Specification
This protocol is focused on being able to transmit a coordinate on the flipper screen and whether it should be on or off. As well as a clear buffer command and a draw command. Every pixel change will be stored in a buffer and when the draw command is sent that buffer will be written to the screen. This will encompass the main basic functionality. Stage 2 will be more complicated and support sending and receiving other data to allow interaction.

**Transmission:**
| Byte # | Content | Hex  |
| ------ | ------- | ---- |
| 1      | STX     | 0x02 |
| 2      | i coord |      |
| 3      | j coord |      |
| 4      | status  |      |
| 5      | ETX     | 0x03 |

### Usage
Setting pixel status (0: off, 1: on):
```
0x02 [i] [j] [0/1] 0x03
```

Flushing the buffer to the screen (draw):
```
0x02 0x00 0x00 0x0f 0x03
```

Clear buffer:
```
0x02 0x00 0x00 0xff 0x03
```

### Examples:
Set pixel (110, 50) on
```
0x02 0x6e 0x32 0x01 0x03
```

Clear buffer
```
0x02 0x00 0x00 0xff 0x03
```

Set pixel (10, 35) off
```
0x02 0x0a 0x23 0x00 0x03
```

Draw to screen
```
0x02 0x00 0x00 0x0f 0x03
```
