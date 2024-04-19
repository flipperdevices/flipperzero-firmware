({
    title: "BadUSB Widget",
    primaryAction: function (buttonNumber) {
        // Do an action based on the button number.
        if (buttonNumber === 1) {
            this.api.badusb.hold("CTRL", "ALT");
            this.api.badusb.press("DELETE");
            this.api.badusb.release("CTRL", "ALT");
            delay(1000);
            this.api.badusb.press("DOWN");
            this.api.badusb.press("DOWN");
            delay(2000);
            this.api.badusb.press("DOWN");
            this.api.badusb.press("ENTER");
        } else if (buttonNumber === 2) {
            // Trigger sticky keys by pressing shift 5 times.
            for (let i = 0; i < 5; i++) {
                this.api.badusb.press("SHIFT");
            }
        } else if (buttonNumber === 4) {
            this.api.badusb.println("SYMBOL TEST `!@#$%^&*()_+-=[]{};'\\:\"|,./<>?", 10);
        } else if (buttonNumber === 8) {
            this.api.badusb.altPrintln("Alt+Numpad  `!@#$%^&*()_+-=[]{};'\\:\"|,./<>?");
        }
    },
    init: function (api) {
        this.api = api;

        this.splashScreen();

        // Initialize access to the Flipper Zero speaker
        this.api.initSpeaker();

        // Initialize access to the BadUSB (virtual keyboard) device
        this.api.initBadusb("prompt");

        this.initWidget();
    },
    splashScreen: function () {
        this.api.require("widget");
        this.api.widget.show();
        let fxbmFlippy = this.api.widget.loadImageXbm(__dirpath + "/flippy.fxbm");
        let splash = [];
        splash.push(this.api.widget.addXbm(0, 0, fxbmFlippy));
        splash.push(this.api.widget.addText(70, 10, "Secondary", "Be sure"));
        splash.push(this.api.widget.addText(70, 20, "Secondary", "to attach"));
        splash.push(this.api.widget.addText(70, 30, "Secondary", "FlipBoard."));
        splash.push(this.api.widget.addText(70, 44, "Secondary", "Connect USB"));
        splash.push(this.api.widget.addText(70, 54, "Secondary", "data cable"));
        splash.push(this.api.widget.addText(70, 64, "Secondary", "to PC."));
        delay(5000);
        for (let i = 0; i < splash.length; i++) {
            this.api.widget.remove(splash[i]);
        }
    },
    initWidget: function () {
        this.api.widget.addText(25, 15, "Primary", this.title);
        this.status = this.api.widget.addText(10, 60, "Secondary", "Press a button!");

        this.fxbmUp = this.api.widget.loadImageXbm(__dirpath + "/up.fxbm");
        this.fxbmDown = this.api.widget.loadImageXbm(__dirpath + "/down.fxbm");
        this.icons = [];

        for (let i = 0; i < 4; i++) {
            this.icons.push(this.api.widget.addXbm(9 + i * 30, 32, this.fxbmUp));
            this.api.widget.addCircle(10 + i * 30 + 3, 36, 10);
        }
    },
    updateWidget: function (buttonNumber, pressedArray) {
        for (let i = 0; i < 4; i++) {
            this.api.widget.remove(this.icons[i]);
            if (pressedArray[i]) {
                this.icons[i] = this.api.widget.addXbm(9 + i * 30, 32, this.fxbmDown);
            } else {
                this.icons[i] = this.api.widget.addXbm(9 + i * 30, 32, this.fxbmUp);
            }
        }
        this.api.widget.remove(this.status);
        this.status = this.api.widget.addText(10, 60, "Secondary", "Button " + to_string(buttonNumber));
    },
    buttonPressed: function (buttonNumber, pressedArray) {
        // Redraw the widget to show the button press.
        this.updateWidget(buttonNumber, pressedArray);

        // A button press of 0 means the user released all of the buttons.
        if (buttonNumber === 0) {
            return;
        }

        // Play a tone for 100ms when button pressed.
        this.api.speaker.play(440 + 100 * buttonNumber, 1.00, 100);

        // Perform the primary action.
        this.primaryAction(buttonNumber);
    }
})