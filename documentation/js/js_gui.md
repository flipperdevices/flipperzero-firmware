# js_gui {#js_gui}

# GUI module
```js
let eventLoop = require("event_loop");
let gui = require("gui");
```

This module depends on the `event_loop` module, so it _must_ only be imported
after `event_loop` is imported.

## Conceptualizing GUI
### Canvas
The canvas is just a drawing area with no abstractions over it. Drawing on the
canvas directly (i.e. not through a viewport) is highly discouraged and
impossible from JS.

### Viewport
A viewport is a window into a rectangular portion of the canvas. Drawing on a
viewport is permitted, but currently inaccessible from JS.

### View
In Flipper's terminology, a "View" is a fullscreen design element that assumes
control over the entire viewport and all input events. Different types of views
are available (not all of which are unfortunately currently implemented in JS):
| View                 | Has JS adapter? |
|----------------------|-----------------|
| `button_menu`        | ❌              |
| `button_panel`       | ❌              |
| `byte_input`         | ❌              |
| `dialog_ex`          | ❌              |
| `empty_screen`       | ✅              |
| `file_browser`       | ❌              |
| `loading`            | ✅              |
| `menu`               | ❌              |
| `number_input`       | ❌              |
| `popup`              | ❌              |
| `submenu`            | ✅              |
| `text_box`           | ✅              |
| `text_input`         | ✅              |
| `variable_item_list` | ❌              |
| `widget`             | ❌              |

### View Dispatcher
The view dispatcher holds references to all the views that an application needs
and switches between them as the application makes requests to do so.

### Scene Manager
The scene manager is an optional add-on to the view dispatcher that makes
managing applications with complex navigation flows easier. It is currently
inaccessible from JS.

### Approaches
In total, there are three different approaches that you may take when writing
a GUI application:
| Approach       | Use cases                                                                    | Available from JS |
|----------------|------------------------------------------------------------------------------|-------------------|
| ViewPort only  | Accessing the graphics API directly, without any of the nice UI abstractions | ❌                |
| ViewDispatcher | Common UI elements that fit with the overall look of the system              | ✅                |
| SceneManager   | Additional navigation flow management for complex applications               | ❌                |

# Example
An example with three different views using the ViewDispatcher approach:
```js
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

// go to the demo chooser screen when the back key is pressed
eventLoop.subscribe(gui.viewDispatcher.navigation, function (_sub, _, gui, demoChooserAssoc) {
    gui.viewDispatcher.switchTo(demoChooserAssoc);
}, gui, demoChooserAssoc);

// run UI
gui.viewDispatcher.switchTo(demoChooserAssoc);
eventLoop.run();
```

# API reference
## `viewDispatcher`
The `viewDispatcher` constant holds the `ViewDispatcher` singleton.

### `ViewDispatcher.add(view)`
Adds a view to the dispatcher, creating and returning a View-ViewDispatcher
association.

#### Parameters
  - `view`: the view to associate with the view dispatcher

#### Returns
An opaque association between the view and the view dispatcher

### `ViewDispatcher.remove(assoc)`
Removes a view from the dispatcher

#### Parameters
  - `assoc`: the View-ViewDispatcher association

### `ViewDispatcher.switchTo(assoc)`
Switches to an associated view, giving it control over the display and input

#### Parameters
  - `assoc`: the View-ViewDispatcher association

### `ViewDispatcher.sendTo(direction)`
Sends the viewport that the dispatcher manages to the front of the stackup
(effectively making it visible), or to the back (effectively making it
invisible)

#### Parameters
  - `direction`: either `"front"` or `"back"`

### `ViewDispatcher.sendCustom(event)`
Sends a custom number to the `custom` event handler

#### Parameters
  - `event`: number to send

### `ViewDispatcher.custom`
An event loop `Contract` object that identifies the custom event source,
triggered by `ViewDispatcher.sendCustom(event)`

### `ViewDispatcher.navigation`
An event loop `Contract` object that identifies the navigation event source,
triggered when the back key is pressed
