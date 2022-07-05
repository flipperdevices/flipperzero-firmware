# flipperzero-nrf24

An [NRF24](https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf) driver for the [Flipper Zero](https://flipperzero.one/) device. The NRF24 is a popular line of 2.4GHz radio transceivers from Nordic Semiconductors. This library is not currently complete, but functional.

## Warning
This repo contains two Flipper Zero apps that utilize the NRF24 driver to sniff for NRF24 addresses and perform mousejack attacks. These apps are for **educational purposes** only. Please use this code responsibly and only use these apps on your own equipment.

## Acknowledgments
The NRF24 sniffing technique was discovered and shared by Travis Goodspeed in [his blog](http://travisgoodspeed.blogspot.com/2011/02/promiscuity-is-nrf24l01s-duty.html).

The mousejack vulnerabilities were discovered and reported by Marc Newlin, see [the blog](https://www.bastille.net/research/vulnerabilities/mousejack/technical-details) for technical details.

Much of the driver code was inspired by [RadioHead's Arduino library](https://www.airspayce.com/mikem/arduino/RadioHead/classRH__NRF24.html).
Much of the mousejack code was inspired by the [Jackit project](https://github.com/insecurityofthings/jackit).
