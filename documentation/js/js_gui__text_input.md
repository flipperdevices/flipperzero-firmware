# js_gui__text_input {#js_gui__text_input}

# Submenu GUI view
```js
let eventLoop = require("event_loop");
let gui = require("gui");
let textInputView = require("gui/text_input");
```

This module depends on the `gui` module, which in turn depends on the
`event_loop` module, so they _must_ be imported in this order.

# Example
For an example refer to the `gui.js` example script.

# API reference
## `make(header, min_length, max_length)`
Makes a `TextInput` object that accepts text of lengths between `min_length` and
`max_length` with the specified `header`.

### Returns
A `TextInput` object

## `TextInput.input`
An event loop `Contract` object that triggers when "save" has been selected in
the input view. The data that the event produces is the input string.
