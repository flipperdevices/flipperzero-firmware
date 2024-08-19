/// <reference types="../../../types/global" />
/// <reference types="../../../types/gpio" />
let gpio = require("gpio");

// initialize pins
let led = gpio.get("pc3"); // same as `gpio.get(7)`
let pot = gpio.get("pc0"); // same as `gpio.get(16)`
let button = gpio.get("pc1"); // same as `gpio.get(15)`
led.init({ direction: "out", outMode: "push_pull" });
pot.init({ direction: "in", inMode: "analog" });
button.init({ direction: "in", pull: "up", inMode: "interrupt", edge: "falling" });

// blink led twice
print("Commencing blinking (PC3)");
led.write(true);
delay(1000);
led.write(false);
delay(1000);
led.write(true);
delay(1000);
led.write(false);

// attach interrupt handler
print("Press the button (PC1)");
button.attach_handler(function (_button, pot) {
    let voltage = pot.read_analog();
    print("PC0 is at", voltage, "mV");
}, pot); // mJS does not support closures, so we pass `pot` via `attach_handler`

// the program will just exit unless this is here
while (true) gpio.process_interrupts(false);

// possible pins https://docs.flipper.net/gpio-and-modules#miFsS
// "PA7" aka 2
// "PA6" aka 3
// "PA4" aka 4
// "PB3" aka 5
// "PB2" aka 6
// "PC3" aka 7
// "PA14" aka 10
// "PA13" aka 12
// "PB6" aka 13
// "PB7" aka 14
// "PC1" aka 15
// "PC0" aka 16
// "PB14" aka 17

// possible modes
// { direction: "out", outMode: "push_pull" }
// { direction: "out", outMode: "open_drain" }
// { direction: "out", outMode: "push_pull", altFn: true }
// { direction: "out", outMode: "open_drain", altFn: true }
// { direction: "in", inMode: "analog" }
// { direction: "in", inMode: "plain_digital" }
// { direction: "in", inMode: "interrupt", edge: "rising" }
// { direction: "in", inMode: "interrupt", edge: "falling" }
// { direction: "in", inMode: "interrupt", edge: "both" }
// { direction: "in", inMode: "event", edge: "rising" }
// { direction: "in", inMode: "event", edge: "falling" }
// { direction: "in", inMode: "event", edge: "both" }
// all variants support an optional `pull` field which can either be undefined,
// "up" or "down"
