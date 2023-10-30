# BadUSB module
```js
let badusb = require("badusb");
```
# Methods
## setup
Start USB HID with optional parameters. Should be called before all other methods.
### Params
Configuration object (optional):
- vid, pid (number): VID and PID values, both are mandatory
- mfr_name (string): Manufacturer name (32  ASCII characters max), optional
- prod_name (string): Product name (32  ASCII characters max), optional

### Example:
```js
// Start USB HID with default parameters
badusb.setup();
// Start USB HID with custom vid:pid = AAAA:BBBB, manufacturer and product strings not defined
badusb.setup({ vid: 0xAAAA, pid: 0xBBBB }); 
// Start USB HID with custom vid:pid = AAAA:BBBB, manufacturer string = "Flipper Devices", product string = "Flipper Zero"
badusb.setup({ vid: 0xAAAA, pid: 0xBBBB, mfr_name: "Flipper Devices", prod_name: "Flipper Zero" });
```

## press
Press and release a key
### Params
Key name, modifier names
//TODO: key codes list

### Example:
```js
badusb.press("a"); // Press "a" key
badusb.press("A"); // SHIFT + "a"
badusb.press("CTRL", "a"); // CTRL + "a"
badusb.press("CTRL", "SHIFT", "ESC"); // CTRL + SHIFT + ESC combo
badusb.press(98); // Press key with HID code (dec) 98 (Numpad 0 / Insert)
badusb.press(0x47); // Press key with HID code (hex) 0x47 (Scroll lock)
```

## hold
### Params
Same as `press`

### Example:
```js
badusb.hold("a"); // Press and hold "a" key
badusb.hold("CTRL", "v"); // Press and hold CTRL + "v" combo
```

## release
### Params
Same as `press`
No params - release all keys

### Example:
```js
badusb.release(); // Release all keys
badusb.release("a"); // Release "a" key
```

## print
Print a string
### Params
- a string to print
- (optional) delay between key presses

### Example:
```js
badusb.print("Hello, world!"); // print "Hello, world!"
badusb.print("Hello, world!", 100); // Add 100ms delay between key presses
```

## println
Same as `print` but ended with "ENTER" press
### Params
- a string to print
- (optional) delay between key presses

### Example:
```js
badusb.println("Hello, world!");  // print "Hello, world!" and press "ENTER"
```