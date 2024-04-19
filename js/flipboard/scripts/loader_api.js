({
    isDefined: function (name) {
        return this[name] !== undefined;
    },
    require: function (name) {
        let lib = undefined;
        if (!this.isDefined(name)) {
            lib = require(name);
            this[name] = lib;
        } else {
            lib = this[name];
        }
        return lib;
    },
    load: function (name, path) {
        let lib = undefined;
        if (!this.isDefined(name)) {
            lib = load(path);
            this[name] = lib;
        } else {
            lib = this[name];
        }
        return lib;
    },
    defaultBadusbLayout: "/ext/badusb/assets/layouts/en-US.kl",
    initBadusb: function (layout_path) {
        // Initialize access to the BadUSB (virtual keyboard) device
        if (!this.isDefined("badusb")) {
            this.require("badusb");
            if (layout_path === undefined) {
                layout_path = this.defaultBadusbLayout;
            } else if (layout_path.charCodeAt(0) !== 0x2F) { // If not an absolute path, prompt for file.
                if (!this.isDefined("dialog")) {
                    this.require("dialog");
                }
                layout_path = this.dialog.pickFile("/ext/badusb/assets/layouts", ".kl");
                if (layout_path === undefined) {
                    layout_path = this.defaultBadusbLayout;
                }
            }
            this.badusb.setup({
                vid: 0x05ac,
                pid: 0x021e,
                mfr_name: "Apple",
                prod_name: "Keyboard",
                layout_path: layout_path
            });
        }
    },
    initSpeaker: function () {
        // Initialize access to the speaker
        if (!this.isDefined("speaker")) {
            this.require("speaker");

            this.speaker.acquire(1000); // NOTE: it will be released when the script exits.
        }
    },
    initSubghz: function () {
        // Initialize access to the Sub-GHz radio
        if (!this.isDefined("subghz")) {
            this.require("subghz");
            this.subghz.setup();
            // For some reason subghz impacts our GPIO pins (so reset them).
            this.flipboardButton.init();
        }
    }
})