let __dirpath = "/ext/apps/Scripts/flipboard";

let loader = load(__dirpath + "/loader_api.js");

// Prompt user for the .fb file to load, this is a FlipBoard script file
let dialog = loader.require("dialog");
let fb = dialog.pickFile(__dirpath, "fb.js");
if (fb === undefined) {
    die("No file selected");
}
loader.load("fn", fb);
print("Loaded", loader.fn.title);

// Initialize textbox
let textbox = loader.require("textbox");
textbox.setConfig("end", "text");
textbox.emptyText();

// Initialize access to the Flipboard buttons
let flipboardButton = loader.load("flipboardButton", __dirpath + "/fb_button_api.js");
flipboardButton.init();

// Initialize access to the Flipboard addressable LEDs
let color = loader.load("color", __dirpath + "/color_api.js");
let flipboardLeds = loader.load("flipboardLeds", __dirpath + "/fb_leds_api.js");
flipboardLeds.init(color, [color.green, color.red, color.yellow, color.blue]);

// Initialize the function callback
loader.fn.init(loader);

// Main loop
let buttonNumber = 0;
while (true) {
    // Wait for a button press
    buttonNumber = flipboardButton.debounceButton(buttonNumber);

    // Convert the button press to an array of LEDs to light up
    let pressedArray = flipboardButton.buttonNumberToArray(buttonNumber);

    // Update the LED brightness.
    flipboardLeds.updateLeds(pressedArray);

    loader.fn.buttonPressed(buttonNumber, pressedArray);
}
