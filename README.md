# Flipper-Zero DCF77 Clock Sync
Emulates the [DCF77](https://en.wikipedia.org/wiki/DCF77) time signal on the RFID antenna and on GPIO A4 pin. 

Uses PWM with frequency of 77.5 kHz on the GPIO pin it to simulate the signal. 

# Usage

Normally the clock gets syncrhonized in two to five minutes depending on the signal strength.

The OK button changes the transmitted signal between CET and CEST (dst) time.

# Antenna
The RFID antenna could be in distance of up to 50cm. The signal gets recognized after few seconds.

When using the GPIO, best results are achieved if you connect a ferrite antenna over 330 ohm resistor and a capactior to ground.

It also works with analog beeper or small in-ear headphone connected to the GPIO pin. 
