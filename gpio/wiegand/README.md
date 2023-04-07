# Wiegand

This application supports W4, W8, W24, W26, W32, W34, W37 and W40 formats.

This application can be used to test Wiegand readers and keypads. It can
save the data to a file, and can load and replay the data. Timings are
measured and displayed; which can be used to help debug Wiegand readers.

## W4: 4-bit Wiegand

This format is used by some keypads. Digits 0-9 are sent as 0-9. ESC is sent as 10 and ENTER as 11. There is no parity bit. The application will display
the button pressed (including ESC and ENTER).

## W8: 8-bit Wiegand

This format is used by some keypads. The last 4 bits are the actual data.Digits 0-9 are sent as 0-9. ESC is sent as 10 and ENTER as 11. The first 4 bits are the inverse of the last 4 bits. The application will display
the button pressed (including ESC and ENTER). If there are any bit errors, the application will show the incorrect bits.

## W26: 26-bit Wiegand

This is a 26-bit format used by many readers. The first bit is an even parity bit. The next 8 bits are the facility code. The next 16 bits are the card number. The last bit is an odd parity bit. The application will display the facility code and card number. If there are any bit errors, the application will show the incorrect bits (the even partity is the first 13 bits, odd parity is the last 13 bits).

## W24: 24-bit Wiegand

This is similar to W26, but without the leading and trailing parity bits. The first 8 bits are the facility code. The next 16 bits are the card number. The application will display the facility code and card number.

## W32/W34/W37/W40: 32/34/37/40-bit Wiegand

These formats are not very standardized, so the application will not try to interpret the data. You can modify the wiegand_data.c file to add your own interpretation.
