# About

MAX31855 (K, J, N, T, S, R, and E)-thermocouple sensor app for flipper zero.

Shows following data:

- Thermocouple temperature
- Internal temperature
- Fault status and flags

# Pinout

This application will work with any chip/thermocouple type, but ensure that you have appropriate combo.
Most common one is MAX31855K and K-type thermocouple.

Also check if your sensor board got it's own LDO for power supply. If there are no LDO on your board then connect sensor to 3.3V.
Boards with LDO are recommended.

Flipper | Sensor
--------|-------
1       | 5V (If your sensor board have LDO)
3       | DO(MISO)
4       | CS
5       | CLK
8       | GND
9       | 3.3V (If your sensor board do not have LDO)
