
# JavaScript scripting API (WIP)
## Note: This documentation is still work in progress! 
Todo:
- Add missing parameters
- Add missing returns
- Add examples

## Description
Flipper supports JavaScript scripting using [mjs](https://github.com/cesanta/mjs). 
**Note: Read about mjs's limitations in the link above! You will experience alot of missing js features (e.g. createTimeout).**

## API
### SubGHZ
`const subghz = require("subghz");`
#### Methods
- setup(): void
- setRx(): void
- setIdle(): void
- getRssi(): number
- getState(): string
- getFrequency(): number
- setFrequency(freq: number): number | undefined
- isExternal(): bool
- transmitFile(file: string): bool | undefined

### Usbdisk
`const usbdisk = require("usbdisk");`
#### Methods
- createImage(file: string, size: number): undefined
- start(file: string): undefined
- stop(): undefined
- wasEjected(): bool | undefined

### BadUsb
`const badusb = require("badusb");`
#### Methods
- setup
- quit
- isConnected
- press
- hold
- release
- print
- println
- 
### BleBeacon
`const blebeacon = require("blebeacon");`
#### Methods
- isActive
- setConfig
- setData
- start
- stop
- keepAlive

### Dialog
`const dialog = require("dialog");`
#### Methods
- message
- custom
- pickFile

### Flipper
`const flipper= require("flipper");`
#### Methods
- getModel
- getName
- getBatteryCharge

### Gpio
`const gpio = require("gpio");`
#### Methods
- init
- write
- read

### Keyboard
`const keyboard = require("keyboard");`
#### Methods
- setHeader
- text
- byte

### Math
`const math = require("math");`
#### Methods
- abs
- acos
- acosh
- asin
- asinh
- atan
- atan2
- atanh
- cbrt
- ceil
- clz32
- cos
- exp
- floor
- log
- max
- min
- pow
- random
- sign
- sin
- sqrt
- trunc
- PI
- E

### Notification
`const notify = require("notification");`
#### Methods
- success
- error
- blink

### Serial
`const serial = require("serial");`
#### Methods
- setup
- write
- read
- readln
- readBytes
- expect

### Storage
`const storage = require("storage");`
#### Methods
- read
- write
- append
- exists
- remove
- virtualInit
- virtualMount
- virtualQuit

### Submenu
`const submenu = require("submenu");`
#### Methods
- addItem
- setHeader
- show
