# js_gui__text_box {#js_gui__text_box}

# Submenu GUI view
```js
let eventLoop = require("event_loop");
let gui = require("gui");
let textBoxView = require("gui/text_box");
```

This module depends on the `gui` module, which in turn depends on the
`event_loop` module, so they _must_ be imported in this order.

# Example
For an example refer to the `gui.js` example script.

# API reference
## `make(font, focus)`
Makes a `TextBox` object

### Parameters
  - `font`: either `"text"` or `"hex"`
  - `focus`: either `"start"` or `"end"`

### Returns
A `TextBox` object

## `TextInput.setText(text)`
Sets the text in the text box
