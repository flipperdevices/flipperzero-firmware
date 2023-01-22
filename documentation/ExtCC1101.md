# External CC1101 module connection:

Tested with module E07-M1101d from Aliexpresss.

Connection (module -> flipper GPIO):

1. GND -> 18
2. VCC -> 9
3. GDO0 -> 5 (B3)
4. CSN -> 6
5. SCK -> 5
6. MOSI -> 2
7. MISO -> 3
8. GD2 (not needed)

The only pin which has to be connected as above is GDO0 (attached to B3). This is due to special function TIM2_2, which 
is the only timer with capture available on external gpio.

All other SPI pins could be redefined by changing the file `furi_hal_spi_sw_config.c`.