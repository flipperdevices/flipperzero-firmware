({
    green: { red: 0x00, green: 0xFF, blue: 0x00 },
    red: { red: 0xFF, green: 0x00, blue: 0x00 },
    yellow: { red: 0xFF, green: 0x7F, blue: 0x00 },
    blue: { red: 0x00, green: 0x00, blue: 0xFF },
    default_glow: 0.10,
    bright_glow: 0.90,
    brightness: function (color, brightness) {
        return {
            red: color.red * brightness,
            green: color.green * brightness,
            blue: color.blue * brightness
        };
    }
})