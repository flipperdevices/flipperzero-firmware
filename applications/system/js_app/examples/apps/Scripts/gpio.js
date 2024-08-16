/// <reference types="../../../types/global" />
/// <reference types="../../../types/gpio" />
let gpio = require("gpio");

// initialize pins
let led = gpio.get("pc3"); // same as `gpio.get(7)`
let button = gpio.get("pc1"); // same as `gpio.get(15)`
led.init({ direction: "out", outMode: "push_pull" });
button.init({ direction: "in", pull: "down", inMode: "interrupt", edge: "rising" });

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
button.attach_handler(function (button) {
    print("Button pressed");
    button.detach_handler();
    print("Press the back button");
});

// the program will just exit unless this is here
while (true) gpio.process_interrupts(true);

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
