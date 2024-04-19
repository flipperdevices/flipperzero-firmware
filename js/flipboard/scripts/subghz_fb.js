({
    title: "SubGHz Sender",
    primaryAction: function (buttonNumber) {
        // Do an action based on the button number.
        if (buttonNumber === 1) {
            this.api.speaker.start(540, 1.00);
            this.api.subghz.transmitFile("/ext/subghz/Light_on.sub");
            this.api.speaker.stop();
        } else if (buttonNumber === 2) {
            this.api.speaker.start(640, 1.00);
            this.api.subghz.transmitFile("/ext/subghz/Light_off.sub");
            this.api.speaker.stop();
        }
    },
    init: function (api) {
        this.api = api;

        // Initialize access to the Flipper Zero speaker
        this.api.initSpeaker();

        // Initialize access to the SubGHz module
        this.api.initSubghz();

        this.initTextbox();
    },
    initTextbox: function () {
        this.api.textbox.addText(this.title + "\n");
        this.api.textbox.addText("Button 1 = Light on.\n");
        this.api.textbox.addText("Button 2 = Light off.\n");
        this.api.textbox.show();
    },
    buttonPressed: function (buttonNumber, pressedArray) {
        // We only use the first two buttons.
        if (buttonNumber !== 1 && buttonNumber !== 2) {
            return;
        }

        // Perform the primary action.
        this.primaryAction(buttonNumber);
    }
})