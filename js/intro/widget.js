let widget = require("widget");

print("Loading file", __filepath);
print("From directory", __dirpath);

let logo = widget.loadImageXbm(__dirpath + "/widget-js.fxbm");

// addText supports "Primary" and "Secondary" font sizes.
widget.addText(10, 10, "Primary", "JavaScript on Flipper");
widget.addText(10, 20, "Secondary", "- How to edit and run scripts");
widget.addText(10, 30, "Secondary", "- Using 'let' and 'require'");
widget.addText(10, 40, "Secondary", "- Different module support");
widget.addText(10, 50, "Secondary", "- badusb, submenu, textbox,");
widget.addText(10, 60, "Secondary", "- storage, keyboard, etc.");
// Show the widget (drawing the layers in the orderer they were added)
widget.show();

let i = 12;
let bitmap = undefined;
while (widget.isOpen()) {
    bitmap = widget.addXbm(7, i, logo);
    delay(500);
    widget.remove(bitmap);
    delay(100);
    i += 10;
    if (i > 60) { i = 12; }
}

// If user did not press the back button, close the widget.
if (widget.isOpen()) {
    widget.close();
}