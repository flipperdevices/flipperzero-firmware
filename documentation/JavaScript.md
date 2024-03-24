
# JavaScript scripting API (WIP)
## Note: This documentation is still work in progress!
Todo:
- Add missing parameters & returns (Dialog and lower)

## Description
Flipper supports JavaScript scripting using [mjs](https://github.com/cesanta/mjs). 
**Note: Read about mjs's limitations in the link above! You will experience alot of missing js features (e.g. createTimeout).**

## Examples
Make sure to check out the [included examples](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/tree/420/applications/system/js_app/examples/apps/Scripts)! They cover basically everything that is possible with Flipper JS.

## API

### Global
- `print(...args: any): undefined`
- `delay(...args: any): undefined | error`
- `to_string(num: number): string`
- `to_hex_string(num: number): string`
- `ffi_address(symbol: string): foreign`
- `require(module: string): object | error`
- `parse_int(text: string): number`
- `to_upper_case(text: string): string | error`
- `to_lower_case(text: string): string | error`

### SubGHZ
`const subghz = require("subghz");`
- `subghz.setup(): undefined`
- `subghz.setRx(): undefined`
- `subghz.setIdle(): undefined`
- `subghz.getRssi(): number`
- `subghz.getState(): string`
- `subghz.getFrequency(): number`
- `subghz.setFrequency(freq: number): number | error`
- `subghz.isExternal(): bool`
- `subghz.transmitFile(file: string): bool | error`

### Usbdisk
`const usbdisk = require("usbdisk");`
- `createImage(file: string, size: number): undefined | error`
- `start(file: string): undefined | error`
- `stop(): undefined | error`
- `wasEjected(): bool | error`

### BadUsb
`const badusb = require("badusb");`
- `setup({ vid: number, pid: number, mfr_name: string, prod_name: string }): undefined | error`
- `quit(): undefined | error`
- `isConnected(): bool | error`
- `press(...keyAndModifiers: string | number): undefined | error`
- `hold(...keyAndModifiers: string | number): undefined | error`
- `release(...keyAndModifiers: string | number | undefined): undefined | error`
- `print(text: string, delay: number | undefined): undefined | error`
- `println(text: string, delay: number | undefined): undefined | error`
- `altPrint(text: string, delay: number | undefined): undefined | error`
- `altPrintln(text: string, delay: number | undefined): undefined | error`

### BleBeacon
`const blebeacon = require("blebeacon");`
- `isActive(): bool | error`
- `setConfig(mac: Uint8Array, power: number | undefined, intvMin: number | undefined, intvMax: number | undefined): undefined | error`
- `setData(data: Uint8Array): undefined | error`
- `start(): undefined | error`
- `stop(): undefined | error`
- `keepAlive(keep: boolean): undefined | error`

### Dialog
`const dialog = require("dialog");`
- `message()`
- `custom()`
- `pickFile()`

### Flipper
`const flipper= require("flipper");`
- `getModel()`
- `getName()`
- `getBatteryCharge()`

### Gpio
`const gpio = require("gpio");`
- `init()`
- `write()`
- `read()`

### Keyboard
`const keyboard = require("keyboard");`
- `setHeader()`
- `text()`
- `byte()`

### Math
`const math = require("math");`
- `abs()`
- `acos()`
- `acosh()`
- `asin()`
- `asinh()`
- `atan()`
- `atan2()`
- `atanh()`
- `cbrt()`
- `ceil()`
- `clz32()`
- `cos()`
- `exp()`
- `floor()`
- `log()`
- `max()`
- `min()`
- `pow()`
- `random()`
- `sign()`
- `sin()`
- `sqrt()`
- `trunc()`
- `PI`
- `E`

### Notification
`const notify = require("notification");`
- `success()`
- `error()`
- `blink()`

### Serial
`const serial = require("serial");`
- `setup()`
- `write()`
- `read()`
- `readln()`
- `readBytes()`
- `expect()`

### Storage
`const storage = require("storage");`
- `read()`
- `write()`
- `append()`
- `exists()`
- `remove()`
- `virtualInit()`
- `virtualMount()`
- `virtualQuit()`

### Submenu
`const submenu = require("submenu");`
- `addItem()`
- `setHeader()`
- `show()`
