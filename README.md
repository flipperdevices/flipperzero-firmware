# BME680 Reader

Expects the device to have address 0x77. Connect SCL to C0, SDA to C1, ground to GND and power to 3V3.

There's [another application](https://github.com/quen0n/unitemp-flipperzero) by [quen0n](https://github.com/quen0n)
with a lot more polish than this, but this updates more frequently, displays results with greater precision and doesn't
have an issue where the humidity is reported always as 100% instead of its actual value.
