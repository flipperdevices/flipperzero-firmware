# js_gui__loading {#js_gui__loading}

# Loading GUI View
```js
let eventLoop = require("event_loop");
let gui = require("gui");
let loadingView = require("gui/loading");
```

This module depends on the `gui` module, which in turn depends on the
`event_loop` module, so they _must_ be imported in this order.

# Example
For an example refer to the GUI example.

# API reference
## `make()`
Makes a Loading view that displays an animated hourglass until it is switched
away from.
