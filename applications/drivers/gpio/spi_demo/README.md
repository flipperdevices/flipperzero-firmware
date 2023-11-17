# SPI_DEMO

This is Not intended to be a full demo, but a simple example of how to use the SPI interface on the Flipper Zero.

Please see [https://youtu.be/W6bGYZ5PgIc](https://youtu.be/W6bGYZ5PgIc) for a video demo.  

My video this Saturday, Nov 18th 2023,  will cover how to wire the device between the Flipper Zero and the chip that supports SPI, such as the W25Q32.

The connections are:
- Flipper Pin 2 - MOSI (D0 on the W25Q32)
- Flipper Pin 3 - MISO (D1 on the W25Q32)
- Flipper Pin 4 - CS (CS on the W25Q32)
- Flipper Pin 5 - SCK (SLK on the W25Q32)
- Flipper Pin 8 - GND (GND on the W25Q32)
- Flipper Pin 9 - 3V3 (VCC on the W25Q32)


The `Hellow-World.sal` file can be viewed using [https://www.saleae.com/downloads/](https://www.saleae.com/downloads/) software.  This is a capture of my W25Q32 chip being read by the Flipper Zero.  The commands were sent using the `SPI Mem Manager` application.  

You can find the `SPI Mem Manager` application at [https://github.com/flipperdevices/flipperzero-good-faps/tree/dev/spi_mem_manager](https://github.com/flipperdevices/flipperzero-good-faps)