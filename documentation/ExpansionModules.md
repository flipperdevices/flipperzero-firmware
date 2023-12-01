# Expansion Module Protocol - Draft

## Terms and definitions

- Expansion module: A third-party hardware unit meant for use with Flipper Zero by connecting it to its GPIO header.
- Expansion module protocol: A serial-based, byte-oriented, synchronous, full-duplex communication protocol described in this document.
- Host: Hardware unit tasked with serving requests. Used interchangeably with Flipper, Flipper Zero, Server etc. throughout this document.
- Device: Used interchangeably with Expansion module, Module, Client, etc.
- RPC: Remote Procedure Call, a protobuf-based communication protocol widely used by Flipper Zero companion applications.
- Timeout interval: Period of inactivity to be treated as a loss of connection, also denoted as Tto. Equals to 250 ms.

## Features

- Automatic expansion module detection
- Baud rate negotiation
- Request-response communication flow
- Integration with Flipper RPC protocol

## Hardware

Depending on the UART selected for communication, the following pins area available for the expansion modules to connect to:

| UART   | Tx pin | Rx pin |
|--------|--------|--------|
| USART  | 13     | 14     |
| LPUART | 15     | 16     |

## Frame structure

Each frame consists of a header (1 byte), contents (size depends of frame type) and CRC field (TBD bytes):

| Header (1 byte) | Contents (0 or more bytes) | CRC (? bytes) |
|-----------------|----------------------------|---------------|
| Frame type      | Frame payload              | CRC checksum  |

### Heartbeat frame

HEARTBEAT frames are used to maintain an idle connection. In the event of not receiving any frames within Tto, either side must cease all communications and be ready to initiate the connection again.

| Header (1 byte) | CRC (? bytes) |
|-----------------|---------------|
| 0x01            | CRC checksum  |

Note that the contents field is not present (0 bytes length).

### Status frame

STATUS frames are used to report the status of a transaction. Every received frame MUST be confirmed by a matching STATUS response.

| Header (1 byte) | Contents (1 byte) | CRC (? bytes) |
|-----------------|-------------------|---------------|
| 0x02            | Error code        | CRC checksum  |

The `Error code` field SHALL have one of the following values:

| Error code | Meaning                 |
|------------|-------------------------|
| 0x00       | OK (No error)           |
| 0x01       | Unknown error           |
| 0x02       | Baud rate not supported |

### Baud rate frame

BAUD RATE frames are used to negotiate communication speed. The initial connection SHALL always happen at 9600 baud. The first message sent by the module MUST be a BAUD RATE frame, even if a different speed is not required.

| Header (1 byte) | Contents (4 bytes) | CRC (? bytes) |
|-----------------|--------------------|---------------|
| 0x03            | Baud rate          | CRC checksum  |

If the requested baud rate is supported by the host, it SHALL respond with a STATUS frame with an OK error code, otherwise the error code SHALL be 0x02 (Baud rate not supported). Until the negotiation succeeds, the speed SHALL remain at 9600 baud. The module MAY send additional BAUD RATE frames with alternative speeds in case the initial request was refused. No other frames are allowed until the speed negotiation succeeds.

### Control frame

CONTROL frames are used to control various aspects of the communication. As of now, the sole purpose of CONTROL frames is to start and stop the RPC session.

| Header (1 byte) | Contents (1 byte) | CRC (? bytes) |
|-----------------|-------------------|---------------|
| 0x04            | Command           | CRC checksum  |

The `Command` field SHALL have one of the followind values:

| Command | Meaning           |
|---------|-------------------|
| 0x00    | Start RPC session |
| 0x01    | Stop RPC session  |

### Data frame

DATA frames are used to transmit arbitrary data in either direction. Each DATA frame can hold up to 64 bytes. If an RPC session is curretly open, all received bytes are forwarded to it.

| Header (1 byte) | Contents (1 to 65 byte(s)) | CRC (? bytes) |
|-----------------|----------------------------|---------------|
| 0x05            | Data                       | CRC checksum  |

The `Data` field SHALL have the following structure:

| Data size (1 byte) | Data (0 to 64 bytes) |
|--------------------|----------------------|
| 0x00 ... 0x40      | Arbitrary data       |

## Communication flow

In order for the host to be able to detect the module, the respective feature must be enabled first. This can be done via the GUI by going to `Settings -> Expansion Modules` and selecting the required `Listen UART` or programmatically by calling `expansion_enable()`. Likewise, disabling this feature via the same GUI or by calling `expansion_disable()` will result in ceasing all communications and not being able to detect any connected modules.

The communication is always initiated by the module by the means of shortly pulling the RX pin down. The host SHALL respond with a HEARTBEAT frame indicating that it is ready to receive requests. The module then MUST issue a BAUDRATE request within Tto. Failure to do so will result in the host dropping the connection and returning to its initial state.

```
        MODULE               |            FLIPPER
-----------------------------+---------------------------
                             |        (Start)
Pull down RX                -->
                            <--       Heartbeat
Baud Rate                   -->
                            <--       Status [OK | Error]
Control [Start RPC]         -->
                            <--       Status [OK | Error]
-----------------------------+--------------------------- (1)
Data [RPC Request]          -->
                            <--       Status [OK | Error]
                            <--       Data [RPC Response]
Status [OK | Error]         -->
-----------------------------+--------------------------- (2)
Data [RPC Request pt.1]     -->
                            <--       Status [OK | Error]
Data [RPC Request pt.2]     -->
                            <--       Status [OK | Error]
Data [RPC Request pt.3]     -->
                            <--       Status [OK | Error]
                            <--       Data [RPC Response]
Status [OK | Error]         -->
-----------------------------+--------------------------- (3)
Heartbeat                   -->
                            <--       Heartbeat
Heartbeat                   -->
                            <--       Heartbeat
-----------------------------+---------------------------
Control [Stop RPC]          -->
                            <--       Status [OK | Error]
(Module disconnected)        |
                             |        (No activity within Tto
                             |            return to start)

(1) The module MUST confirm all implicitly requested frames (e.g. DATA frames containing RPC responses) with a STATUS frame.
(2) RPC requests larger than 64 bytes are split into multiple frames. Every DATA frame MUST be confirmed with a STATUS frame.
(3) When the module has no data to send, it MUST send HEARTBEAT frames with a period < Tto in order to maintain the connection.
    The host SHALL respond with a HEARTBEAT frame each time.
```

(To be continued...)
