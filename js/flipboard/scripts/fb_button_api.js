({
    gpio: require("gpio"),
    button_pins: ["PB2", "PB3", "PA4", "PA6"],
    repeat: false,
    init: function () {
        for (let i = 0; i < this.button_pins.length; i++) {
            this.gpio.init(this.button_pins[i], "input", "up");
        }
    },
    getButtons: function () {
        let n = 0;
        for (let i = 0; i < this.button_pins.length; i++) {
            let isPressed = !this.gpio.read(this.button_pins[i]);
            n += isPressed ? 1 << i : 0;
        }

        return n;
    },
    debounceButton: function (button) {
        let threshold = 3;
        let repeatThreshold = 5;
        let debounce = { counter: threshold, button: button & ~16 };
        while (true) {
            let button = this.getButtons();
            if (button !== debounce.button) {
                debounce.counter = 0;
                debounce.button = button;
                continue;
            } else {
                debounce.counter++;
                if (debounce.counter === threshold) {
                    break;
                } else if (debounce.counter < threshold) {
                    continue;
                } else if (debounce.counter > threshold) {
                    if (this.repeat && debounce.counter > repeatThreshold) {
                        debounce.button |= 16;
                        break;
                    }
                    delay(1);
                    continue;
                }
            }
        }

        return debounce.button;
    },
    buttonNumberToArray: function (button_number) {
        let binary_array = [];
        for (let i = 0; i < this.button_pins.length; i++) {
            binary_array.push((button_number >> i) & 1);
        }
        return binary_array;
    }
})