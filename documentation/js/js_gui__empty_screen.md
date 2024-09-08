# js_gui__empty_screen {#js_gui__empty_screen}

# Empty Screen GUI View
```js
let eventLoop = require("event_loop");
let gui = require("gui");
let emptyView = require("gui/empty_screen");
```

This module depends on the `gui` module, which in turn depends on the
`event_loop` module, so they _must_ be imported in this order.

# Example
For an example refer to the GUI example.

# API reference
## `make()`
Makes an Empty Screen view that displays nothing until it is switched away from.
