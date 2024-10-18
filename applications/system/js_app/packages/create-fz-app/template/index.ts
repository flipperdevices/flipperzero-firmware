// This app will run on official firmware that has a JS API version of 0.x, but
// no less than 0.1.
// It is HEAVILY RECOMMENDED to check which JS SDK version an API that you want
// to use was added in. Also see: `sdkCompatibilityStatus`, `isSdkCompatible`.
assertSdkCompatibility(0, 1, "flipperdevices");

// import modules
// caution: `eventLoop` HAS to be imported before `gui`, and `gui` HAS to be
// imported before any `gui` submodules.
import * as eventLoop from "@flipperdevices/fz-sdk/event_loop";
import * as gui from "@flipperdevices/fz-sdk/gui";
import * as dialog from "@flipperdevices/fz-sdk/gui/dialog";

// a common pattern is to declare all the views that your app uses on one object
const views = {
    dialog: dialog.makeWith({
        header: "Hello from <app_name>",
        text: "Check out index.ts and\nchange something :)",
        center: "Gonna do that!",
    }),
};

// stop app on center button press
eventLoop.subscribe(views.dialog.input, (_sub, button, eventLoop) => {
    if (button === "center")
        eventLoop.stop();
}, eventLoop);

// stop app on back button press
eventLoop.subscribe(gui.viewDispatcher.navigation, (_sub, _item, eventLoop) => {
    eventLoop.stop();
}, eventLoop);

// run app
gui.viewDispatcher.switchTo(views.dialog);
eventLoop.run();
