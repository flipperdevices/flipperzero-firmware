/// <reference types="../../../types/global" />
/// <reference types="../../../types/event_loop" />
let event_loop = require("event_loop");
/// <reference types="../../../types/gui" />
let gui = require("gui");
/// <reference types="../../../types/gui/loading" />
let loadingView = require("gui/loading");

let loading = loadingView.make();
let loadingAssoc = gui.viewDispatcher.add(loading);

event_loop.subscribe(gui.viewDispatcher.event("custom"), function (_sub, item) {
    print(item);
});

event_loop.subscribe(gui.viewDispatcher.event("navigation"), function (_sub, _item, gui) {
    gui.viewDispatcher.send_custom(123);
}, gui);

event_loop.subscribe(event_loop.timer("oneshot", 1000), function (_sub, _item, loop) {
    loop.stop();
}, event_loop);

gui.viewDispatcher.switch_to(loadingAssoc);
event_loop.run();
