({
    title: "BadUSB Textbox",
    primaryAction: function (buttonNumber) {
        // Do an action based on the button number.
        if (buttonNumber === 1) {
            this.api.badusb.println("https://www.youtube.com/@MrDerekJamison/playlists", 10);
        } else if (buttonNumber === 2) {
            this.api.badusb.print("Flipper Name: ", 10);
            this.api.require("flipper");
            this.api.badusb.println(this.api.flipper.getName(), 10);
        } else if (buttonNumber === 4) {
            this.api.badusb.println("I TYPE SLOW!", 250);
        } else if (buttonNumber === 8) {
            this.api.badusb.altPrintln("This was printed with Alt+Numpad method!");
        }
    },
    init: function (api) {
        this.api = api;

        // Initialize access to the Flipper Zero speaker
        this.api.initSpeaker();

        // Initialize access to the BadUSB (virtual keyboard) device
        this.api.initBadusb("/ext/badusb/assets/layouts/en-US.kl");

        this.initTextbox();
    },
    initTextbox: function () {
        this.api.textbox.addText(this.title + "\n");
        this.api.textbox.addText("Press a button.\n");
        this.api.textbox.show();
    },
    updateTextbox: function (buttonNumber, pressedArray) {
        let text = "Button " + to_string(buttonNumber) + " pressed: ";
        for (let i = 0; i < 4; i++) {
            text += pressedArray[i] ? "X" : "_";
        }
        text += "\n";
        this.api.textbox.addText(text);
    },
    buttonPressed: function (buttonNumber, pressedArray) {
        // Redraw the textbox to show the button press.
        this.updateTextbox(buttonNumber, pressedArray);

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