# js_gui__widget {#js_gui__widget}

# Widget GUI view
Displays a combination of custom elements on one screen

<img src="widget.png" width="200" alt="Sample screenshot of the view" />

```js
let eventLoop = require("event_loop");
let gui = require("gui");
let widgetView = require("gui/widget");
```

This module depends on the `gui` module, which in turn depends on the
`event_loop` module, so they _must_ be imported in this order. It is also
recommended to conceptualize these modules first before using this one.

# Example
For an example refer to the `gui.js` example script.

# View props
This view does not have any props.

# Children
This view has the elements as its children.
