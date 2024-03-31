// Description: This script demonstrates the usage of the Flipper JavaScript API.

// For more information, please visit my Flipper Zero Wiki:
// https://www.github.com/jamisonderek/flipper-zero-tutorials/wiki/JavaScript

// Only the "let" keyword is supported for variable declarations.

// The "require" function is used to import modules.  The list of available modules varies by firmware version.
let badusb = require("badusb");
let gpio = require("gpio");
let keyboard = require("keyboard");
let storage = require("storage");
let subghz = require("subghz");
let submenu = require("submenu");
let textbox = require("textbox");
let usbdisk = require("usbdisk");

// Sub-GHz radio setup
subghz.setup();

// initialize GPIO pins as Input pins with internal pull-up resisters.
let buttons = ["PB2", "PB3", "PA4", "PA6"];
for (let i = 0; i < buttons.length; i++) {
    gpio.init(buttons[i], "input", "up");
}

// returns true if the button is pressed (e.g. pin is connected to ground).
function isPressed(button) {
    return !gpio.read(buttons[button]);
}

// Types the given text using the BadUSB module.
function typeText(text) {
    badusb.setup({ vid: 0x1234, pid: 0x5678, mfr_name: "Apple", prod_name: "Keyboard" });

    // We need to wait for up to 10 seconds for the BadUSB to connect.
    for (let retry = 10; retry > 0; retry--) {
        if (badusb.isConnected()) {
            break; // Exit the loop if the BadUSB is connected
        }
        delay(1000); // Wait for 1 second
    }

    if (badusb.isConnected()) {
        badusb.press("GUI", "r"); // Win + R
        delay(500); // Wait for the Run dialog to appear
        badusb.println(text); // Type the text into the Run dialog and press Enter
    } else {
        print("BadUSB not connected", 1000); // Print an error message if the BadUSB is not connected and wait for 1 second
    }

    badusb.quit(); // Disconnect the BadUSB device
}

// Returns the list of button names.
function pinNames() {
    let names = "";

    for (let i = 0; i < buttons.length; i++) {
        names += buttons[i] + "  ";
    }

    return names;
}

// Use the FlipBoard module to bind GPIO buttons to actions.
function demoFlipBoard() {

    // Set the configuration for the textbox
    textbox.setConfig("end", "text"); // Focus (start / end), Font (text / hex)
    textbox.emptyText();
    textbox.addText("FlipBoard demo\nMonitoring button presses.\npins:" + pinNames() + "\nPress back to exit.");
    textbox.show(); // non-blocking

    while (textbox.isOpen()) { // User can close the textbox by pressing the back button
        if (isPressed(0)) {
            typeText("https://github.com/jamisonderek/flipper-zero-tutorials/wiki/JavaScript");
        } else if (isPressed(1)) {
            typeText("https://th.bing.com/th/id/OIG2.VuhdedoFVZRvTzl6FO4F?pid=ImgGn");
        } else if (isPressed(2)) {
            typeText("https://youtu.be/xvFZjo5PgG0");
        } else if (isPressed(3)) {
            let result = subghz.transmitFile("/ext/subghz/Light_on.sub");
            if (!result) {
                print("Send failed");
            }
        }

        delay(100);
    }
}

// Parses an integer from the given text and counts the next four numbers.
function demoCount() {
    keyboard.setHeader("Starting count");
    let text = keyboard.text(100, "1337", false);
    if (text === undefined) {
        return; // User pressed the back button
    }

    print("Text:", text);
    let count = parse_int(text);
    for (let i = 0; i < 4; i++) {
        print("Count:", count + i);
    }
    print("Counting done!\n");
}

// Creates a 4MB image file and mounts it as a USB Mass Storage device.
function demoMassStorage() {
    // NOTE: The __dirpath variable (is the location of the current script) but it may not be defined in your firmware.
    let path = __dirpath + "/4MB.img";
    let size = 4 * 1024 * 1024; // 4MB
    if (!storage.exists(path)) {
        print("Creating image...");
        usbdisk.createImage(path, size); // Create a 4MB image file
    }
    print("Starting UsbDisk...");
    usbdisk.start(path); // Mount the image as a USB Mass Storage device
    print("Started, waiting until ejected...");
    while (!usbdisk.wasEjected()) { // Wait until the USB Mass Storage device is ejected from the host
        delay(1000);
    }
    print("Ejected, stopping UsbDisk...");
    usbdisk.stop(); // Unmount the USB Mass Storage device
    print("Done");
}

while (true) {
    submenu.setHeader("Select awesome demo:");
    submenu.addItem("Counting demo", 0);
    submenu.addItem("FlipBoard GPIO demo", 1);
    submenu.addItem("Mass Storage", 2);
    let result = submenu.show();
    if (result === undefined) { // User pressed the back button
        break;
    }

    if (result === 0) {
        demoCount();
    } else if (result === 1) {
        demoFlipBoard();
    } else if (result === 2) {
        demoMassStorage();
    }

    delay(4 * 1000); // Wait four seconds before showing the menu again
};