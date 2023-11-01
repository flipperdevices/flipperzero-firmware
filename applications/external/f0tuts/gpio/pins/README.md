# Pins

## Description
To help making things easier, I have been trying to use specific colored wires for connecting to various devices.  I also made a breakout board that connects to the top of the Flipper Zero and exposes the same colored wires on a breadboard for quick prototyping. The breadboard also exposes a copy of each general output with a 220 ohm series resistor (which is helpful for driving LEDs).

The pins have different names depending on their meaning, so this
helps me connect to the proper pins.

I use the following colors (for some usages like LPUART v. I2C it isn't a match; but I think it is more common for the pins to be I2C):
- White = +5V
- Red = +3.3V
- Black = GND
- Orange = OUT [from Flipper perspective]
- Indigo = IN [from Flipper perspective]
- Yellow = IN/OUT
- Green = SELECT [TYPICALLY LOW]
- Brown = CLOCK
- Blue = GENERAL/IRQ
- Grey = TYPICALLY UNUSED
NOTE: For General Purpose Input/Output (like controlling a 7-segment LED display) instead of a specific protocol, the majority of the colors have no meanings.


## Installing
I copy these files into a folder on my Flipper Zero (like "pins") so that I can easily browse all of the pins.  If you click on a file and chose "Info" so that you see the full name without scrolling.  If you click on a file and choose "Show" then you will see details about that pin.

## Future work
Many devices have printing on the underside, so you have to be careful when connecting to those (or connect wires from breadboard to device - instead of plugging in directly to device.)  Some devices have two sets of adjecent pins, so they don't work in breadboard (I'm working on making an adapter soon) so they can go into my breadboard (but you can also connect wires from breadboard to device.)
