let eventLoop = require("event_loop");
let gui = require("gui");
let loadingView = require("gui/loading");
let submenuView = require("gui/submenu");
let emptyView = require("gui/empty_screen");

// loading screen
let loading = loadingView.make();
let loadingAssoc = gui.viewDispatcher.add(loading);

// empty screen
let empty = emptyView.make();
let emptyAssoc = gui.viewDispatcher.add(empty);

// demo chooser screen
let demoChooser = submenuView.make();
demoChooser.setItems([
    "Hourglass screen",
    "Empty screen",
    "Exit app",
]);
demoChooser.setHeader("Choose demo");
let demoChooserAssoc = gui.viewDispatcher.add(demoChooser);
eventLoop.subscribe(demoChooser.chosen, function (_sub, index, gui, eventLoop, loadingAssoc, emptyAssoc, demoChooserAssoc) {
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
        eventLoop.stop();
    }
}, gui, eventLoop, loadingAssoc, emptyAssoc, demoChooserAssoc);

// go to the demo chooser screen when the back key is pressed
eventLoop.subscribe(gui.viewDispatcher.navigation, function (_sub, _, gui, demoChooserAssoc) {
    gui.viewDispatcher.switchTo(demoChooserAssoc);
}, gui, demoChooserAssoc);

// run UI
gui.viewDispatcher.switchTo(demoChooserAssoc);
eventLoop.run();
