# js_gui__submenu {#js_gui__submenu}

# Submenu GUI view
```js
let eventLoop = require("event_loop");
let gui = require("gui");
let submenuView = require("gui/submenu");
```

This module depends on the `gui` module, which in turn depends on the
`event_loop` module, so they _must_ be imported in this order.

# Example
For an example refer to the GUI example.

# API reference
## `make()`
Makes a `Submenu` object

## `Submenu.setItems(items)`

### Parameters
  - `items`: list of strings that represent the entries of the menu

## `Submenu.setHeader(header)`

### Parameters
  - `header`: optional title of the menu

## `Submenu.chosen`
An event loop `Contract` object that triggers when an item has been chosen.
The data that the event produces is the index of the chosen item.
