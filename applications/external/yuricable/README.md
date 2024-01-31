# YuriCable Pro Max

Implementation to use a Flipper-Zero as SWD/DCSD-Cable for iPhones just like known Kanzi/Kong/Tamarin Cables

> ![](docs/YuriCableLogo.svg)

## Tested iPhones:

| iPhone      | DCSD | Reset | DFU | JTAG |
| ----------- | --- | ------ | --- | ---- |
| 14 Pro Max  |  ✅  |  ✅  |  ✅  |  ❌   |
| 14 Pro      |  ✅  |  ✅  |  ✅  |  ❌   |
| SE (2022)   |  ✅  |  ✅  |  ✅  |  ❌   |
| XS          |  ❌  |  ❌  |  ❌  |  ❌   |

## Thanks

### SDQ

Here some specs about the protocol used:

+ Name: SDQ (IDBUS) developed by Texas Instruments
+ Source: [Reversed Protocol](https://nyansatan.github.io/lightning/)

Credits for SDQ reverse engineering to [@nyansatan](https://github.com/nyansatan)

### Tamarin Cable Implementation

+ [Tamarin Firmware](https://github.com/stacksmashing/tamarin-firmware)

Credits to [@stacksmashing](https://github.com/stacksmashing) for an example pi pico implementation and his defcon talk
on this subject. (
watch [here](https://www.youtube.com/watch?v=8p3Oi4DL0eI&list=PL0P69gP-VL8eSCSNY-gQefgY1DXBSlNJC&index=6&))


## Project Setup

### Create .vscode

```shell
ufbt vscode_dist
```

### Open in CLion

Open the Project in CLion

## Flipper Docs

[Flipper Docs](https://docs.flipper.net/)

## Pinout Flipper Zero

![](docs/FlipperZeroPinout.jpg)

## Pinout Lightning

Breakout Color Coding

![](docs/LightningBreakoutPinout.jpg)

Female Port Pinout

![](docs/Connector2.jpg)

USB UART Pinout

![](docs/Connector1.jpg)

## Build

### Build-Tool

#### Install Build-Tool

```shell
python -m pip install --upgrade ufbt
```

#### Update Firmware

+ Download Update

```shell
ufbt update --channel=release
```

#### Build App FLash it

+ Upload to Flipper

```shell
ufbt flash_usb
```

+ VSCode setup and build

```shell
ufbt faps vscode_dist
```

#### Build FAP

Navigiere in das Rootverzeichnis der App und führe dort nach installation von ufbt folgenden Befehl aus:

```shell
ufbt
```

Die `.fap`-Datei liegt dann im `./dist`-Ordner

#### Auto Launch

```shell
ufbt launch
```

## Debug

### ST-Link

+ [Older Guide for ST-Link](https://www.drewgreen.net/debugging-flipper-zero-firmware-with-st-link-v2/)

### JLink

#### Install JLINK Software

[Download latest JLINK Software](https://www.segger.com/downloads/jlink/)

Then update the Firmware of the JLink.

#### Pinout

+ Pinoout of the Segger J-Link

<img src="docs/JLINK.png" alt="JLINK" height="500px"/>

#### Pin Mapping

Mapping Table:

```txt
Flipper GPIO |  JLINK GPIO
__________________________
12 (SIO)     |  7 (TMS)
11 (GND)     |  8 (GND)
10 (SWC)     |  9 (TCK)
09 (3v3)     |  1 (VTref)
```

<img src="docs/FlipperJLINK.jpg" alt="FlipperJLINK" height="850px"/>

or buy a JTAG Adapter for Flipper Zero by [@jrozner](https://twitter.com/jrozner)
on [Tindie](https://www.tindie.com/products/cuttrace/flipper-zero-jlink-adapter/) and extend the GPIO-Pins

<img src="docs/flipper_jlink_adapter.jpg" alt="FlipperJLINKAdapter" height="850px"/>

## Docs

### UFBT

+ Repo:  [UFBT GitHub](https://github.com/flipperdevices/flipperzero-ufbt)
+ Docs:  [UFBT DOCS](https://github.com/flipperdevices/flipperzero-ufbt/blob/dev/README.md)

### ARM Stuff

+ DWT_CYCCNT
  explained: [ARM DOCS](https://developer.arm.com/documentation/ddi0403/d/Debug-Architecture/ARMv7-M-Debug/The-Data-Watchpoint-and-Trace-unit/CYCCNT-cycle-counter-and-related-timers?lang=en)

## Class Diagramm

```plantuml
@startuml

skinparam linetype polyline
skinparam linetype ortho

class SDQDevice {
    + const GpioPin* gpio_pin
    + const UsbUartBridge* uart_bridge
    + SDQTimings timings
    + SDQDeviceError error
    + SDQDeviceCommand runCommand
    + bool listening
    + bool connected
    + bool resetInProgress
    + SDQDeviceCommandCallback command_callback
    + command_callback_context: void*
    void start()
    void stop()
    bool send(const uint8_t data[], size_t data_size)
    bool receive(uint8_t data[], size_t data_size)
}

enum SDQDeviceError {
    SDQDeviceErrorNone
    SDQDeviceErrorResetInProgress
    SDQDeviceErrorPresenceConflict
    SDQDeviceErrorInvalidCommand
    SDQDeviceErrorBitReadTiming
    SDQDeviceErrorTimeout
}

enum SDQDeviceCommand {
    SDQDeviceCommand_NONE
    SDQDeviceCommand_DCSD
    SDQDeviceCommand_JTAG
    SDQDeviceCommand_DFU
    SDQDeviceCommand_RECOVERY
    SDQDeviceCommand_USB_A_CHARGING_CABLE
}

class SDQTimings {
    + uint32_t BREAK_meaningful_min
    + uint32_t BREAK_meaningful_max
    + uint32_t BREAK_meaningful
    + uint32_t BREAK_recovery
    + uint32_t WAKE_meaningful_min
    + uint32_t WAKE_meaningful_max
    + uint32_t WAKE_meaningful
    + uint32_t WAKE_recovery
    + uint32_t ZERO_meaningful_min
    + uint32_t ZERO_meaningful_max
    + uint32_t ZERO_meaningful
    + uint32_t ZERO_recovery
    + uint32_t ONE_meaningful_min
    + uint32_t ONE_meaningful_max
    + uint32_t ONE_meaningful
    + uint32_t ONE_recovery
    + uint32_t ZERO_STOP_recovery
    + uint32_t ONE_STOP_recovery
}

class TRISTART_RESPONSES {
    DFU: uint8_t[7]
    RESET_DEVICE: uint8_t[7]
    USB_UART_JTAG: uint8_t[7]
    USB_SPAM_JTAG: uint8_t[7]
    USB_UART: uint8_t[7]
    USB_A_CHARGING_CABLE: uint8_t[7]
}

class UsbUartBridge {

}

interface yuricable_pro_max {
    + const GpioPin SDQ_PIN
    {static} void demo_input_callback(InputEvent* input_event, FuriMessageQueue* queue)
    {static} void demo_render_callback(Canvas* canvas, void* ctx)
    int32_t yuricable_pro_max_app(void* p)
}

SDQDevice *-- SDQTimings
SDQDevice *-- TRISTART_RESPONSES
SDQDevice <-left- SDQDeviceError
SDQDevice <-left- SDQDeviceCommand
SDQDevice +-right- yuricable_pro_max
SDQDevice +-right- UsbUartBridge

@enduml
```

## Gantt

```plantuml
@startgantt
Labels on first column
saturday are closed
sunday are closed
project starts 2024-01-10 
[M1] happens D+1
[M2] happens D+6
[Docu] requires 5 day and starts D+1
-- Flipper --
[SDQ Implementation] requires 2 day
[CLI Commands] requires 2 day and starts at [SDQ Implementation]s end
[UART Implementation] requires 1 day and starts at [SDQ Implementation]s end
[SWD Implementation] requires 1 day and starts at [UART Implementation]s end
[JTAG Implementation] requires 1 day and starts at [SWD Implementation]s end
-- Monitor --
[Explor WebSerial] requires 2 day
[Create React App] requires 1 day and starts at [Explor WebSerial]s end
[Implement WebSerial] requires 1 day and starts at [Create React App]s end
[Command Buttons] requires 1 day and starts at [Implement WebSerial]s end
[Device Filter] requires 1 day and starts at [Implement WebSerial]s end
@endgantt
```
