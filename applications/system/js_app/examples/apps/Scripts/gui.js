let eventLoop = require("event_loop");
let gui = require("gui");
let loadingView = require("gui/loading");
let submenuView = require("gui/submenu");
let emptyView = require("gui/empty_screen");
let textInputView = require("gui/text_input");
let textBoxView = require("gui/text_box");

// loading screen
let loading = loadingView.make();
let loadingAssoc = gui.viewDispatcher.add(loading);

// empty screen
let empty = emptyView.make();
let emptyAssoc = gui.viewDispatcher.add(empty);

// text input
let keyboard = textInputView.make("Enter your name", 0, 32);
let keyboardAssoc = gui.viewDispatcher.add(keyboard);

// text box
let textBox = textBoxView.make("text", "start");
let textBoxAssoc = gui.viewDispatcher.add(textBox);

// demo chooser screen
let demoChooser = submenuView.make();
demoChooser.setItems([
    "Hourglass screen",
    "Empty screen",
    "Text input",
    "Exit app",
]);
demoChooser.setHeader("Choose demo");
let demoChooserAssoc = gui.viewDispatcher.add(demoChooser);
eventLoop.subscribe(demoChooser.chosen, function (_sub, index, gui, eventLoop, loadingAssoc, emptyAssoc, demoChooserAssoc, keyboardAssoc) {
    if (index === 0) {
        gui.viewDispatcher.switchTo(loadingAssoc);
        // the loading view captures all back events, preventing our navigation callback from firing
        // switch to the demo chooser after a second
        eventLoop.subscribe(eventLoop.timer("oneshot", 1000), function (_sub, _, gui, demoChooserAssoc) {
            gui.viewDispatcher.switchTo(demoChooserAssoc);
        }, gui, demoChooserAssoc);
    } else if (index === 1) {
        gui.viewDispatcher.switchTo(emptyAssoc);
    } else if (index === 2) {
        gui.viewDispatcher.switchTo(keyboardAssoc);
    } else if (index === 3) {
        eventLoop.stop();
    }
}, gui, eventLoop, loadingAssoc, emptyAssoc, demoChooserAssoc, keyboardAssoc);

// go to the demo chooser screen when the back key is pressed
eventLoop.subscribe(gui.viewDispatcher.navigation, function (_sub, _, gui, demoChooserAssoc) {
    gui.viewDispatcher.switchTo(demoChooserAssoc);
}, gui, demoChooserAssoc);

// print text from the keyboard
eventLoop.subscribe(keyboard.input, function (_sub, name, gui, textBox, textBoxAssoc) {
    textBox.setText("Hello, " + name + "! Nice to meet you");
    gui.viewDispatcher.switchTo(textBoxAssoc);
}, gui, textBox, textBoxAssoc);

// run UI
gui.viewDispatcher.switchTo(demoChooserAssoc);
eventLoop.run();
