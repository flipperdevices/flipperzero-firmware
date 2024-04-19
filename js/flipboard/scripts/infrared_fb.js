({
    title: "Infrared Blast",
    primaryAction: function (buttonNumber) {
        // Do an action based on the button number.
        if (buttonNumber === 1) {
            this.api.textbox.addText("\nPower");
            this.api.infrared.sendProtocol("Samsung32", 0x07, 0x02);
        } else if (buttonNumber === 2) {
            this.api.textbox.addText("\nVolume Up");
            this.api.infrared.sendProtocol("Samsung32", 0x07, 0x07);
        } else if (buttonNumber === 4) {
            this.api.textbox.addText("\nVolume Down");
            this.api.infrared.sendProtocol("Samsung32", 0x07, 0x0B);
        } else if (buttonNumber === (2 | 8)) {
            this.api.textbox.addText("\nChannel Up");
            this.api.infrared.sendProtocol("Samsung32", 0x07, 0x12);
        } else if (buttonNumber === (4 | 8)) {
            this.api.textbox.addText("\nChannel Down");
            this.api.infrared.sendProtocol("Samsung32", 0x07, 0x10);
        }
    },
    init: function (api) {
        this.api = api;

        // Allow the Flipboard button to repeat action [adding button 16] when held down.
        this.api.flipboardButton.repeat = true;

        // Initialize access to the Flipper Zero speaker
        this.api.initSpeaker();

        // Initialize access to the Infrared module
        this.api.require("infrared");

        this.initTextbox();
    },
    initTextbox: function () {
        this.api.textbox.addText(this.title + "\n");
        this.api.textbox.addText("Green: Power.\nRed = Volume +\nYellow = Volume -\nBlue + Red = Channel +\nBlue + Yellow = Channel -");
        this.api.textbox.show();
    },
    buttonPressed: function (buttonNumber, _pressedArray) {
        // Ignore button 1 [Power] if repeat (16).
        if (buttonNumber === (1 | 16)) {
            return;
        }

        // Remove the repeat flag.
        buttonNumber = buttonNumber & ~16;

        // A button press of 0 means the user released all of the buttons.
        if (buttonNumber === 0) {
            return;
        }

        // Ignore holding button 8
        if (buttonNumber === 8) {
            return;
        }

        // Play a tone for 100ms when button pressed.
        this.api.speaker.play(440 + 100 * buttonNumber, 1.00, 100);

        // Perform the primary action.
        this.primaryAction(buttonNumber);
    }
})