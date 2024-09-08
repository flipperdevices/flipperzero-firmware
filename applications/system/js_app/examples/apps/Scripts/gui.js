/// <reference types="../../../types/global" />
/// <reference types="../../../types/event_loop" />
let eventLoop = require("event_loop");
/// <reference types="../../../types/gui" />
let gui = require("gui");
/// <reference types="../../../types/gui/loading" />
let loadingView = require("gui/loading");
/// <reference types="../../../types/gui/submenu" />
let submenuView = require("gui/submenu");

// loading screen
let loading = loadingView.make();
let loadingAssoc = gui.viewDispatcher.add(loading);

// demo chooser screen
let demoChooser = submenuView.make();
demoChooser.setItems([
    "Hourglass screen",
    "Empty screen",
    "Exit app",
]);
eventLoop.subscribe(demoChooser.chosen, function (_sub, index, gui, eventLoop, loadingAssoc) {
    if (index === 0) {
        gui.viewDispatcher.switchTo(loadingAssoc);
    } else if (index === 1) {
        print("not implemented");
    } else if (index === 2) {
        eventLoop.stop();
    }
}, gui, eventLoop, loadingAssoc);
let demoChooserAssoc = gui.viewDispatcher.add(demoChooser);

// run UI
gui.viewDispatcher.switchTo(demoChooserAssoc);
eventLoop.run();
