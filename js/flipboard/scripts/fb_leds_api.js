({
    color_api: undefined,
    rgbLeds: require("rgbleds"),
    led_colors: [],
    updateLeds: function (bright) {
        let isChanged = false;
        for (let i = 0; i < this.led_colors.length; i++) {
            let b = bright[i] ? this.color_api.bright_glow : this.color_api.default_glow;
            let c = this.color_api.brightness(this.led_colors[i], b); // using global 'color' object.
            if (this.rgbLeds.set(i, c) !== c) {
                isChanged = true;
            }
        }
        // We always call update, so LEDs can be unplugged and reconnected.
        this.rgbLeds.update();
        return isChanged;
    },
    init: function (color_api, colors) {
        this.color_api = color_api;
        this.led_colors = colors;
        this.rgbLeds.setup({ "pin": "PC3", "count": this.led_colors.length, "spec": "WS2812B" });
        let state = [];
        for (let i = 0; i < this.led_colors.length; i++) {
            state.push(false);
        }
        this.updateLeds(state);
    }
})