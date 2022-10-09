# Stage 1 Communication Protocol (v1.0.0)
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

## Protocol Specification
Start byte: STX (0x02) <br>
End byte:   ETX (0x03)