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
eventLoop.subscribe(demoChooser.chosen, function (_sub, index, gui, eventLoop, loadingAssoc, emptyAssoc) {
    if (index === 0) {
        gui.viewDispatcher.switchTo(loadingAssoc);
    } else if (index === 1) {
        gui.viewDispatcher.switchTo(emptyAssoc);
    } else if (index === 2) {
        eventLoop.stop();
    }
}, gui, eventLoop, loadingAssoc, emptyAssoc);
let demoChooserAssoc = gui.viewDispatcher.add(demoChooser);

// run UI
gui.viewDispatcher.switchTo(demoChooserAssoc);
eventLoop.run();
