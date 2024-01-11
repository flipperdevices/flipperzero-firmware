# Flipperzero -NRF24 channel scanner

## Author: [htotoo](https://github.com/htotoo/NRF24ChannelScanner)

## What is it?
This app will scan all the NRF24's frequencies, and shows where it found any traffic. The more traffic, the higher the bars.
After the scanning finished, you can hit left or right keys, to find what frequency is in use.

## How to use
- Connect NRF24 to flipper using provided pinouts
- Open NRF24 Channel scanner, and hit OK
- Done

## Screenshot

<img src="https://raw.githubusercontent.com/htotoo/NRF24ChannelScanner/main/NRF24ChannelScanner/.flipcorg/gallery/screen1.png">

## Buttons

OK short: start / stop the scan.

OK long: start an infinite scan, that scans until stopped.

BACK long: exit the app.

LEFT / RIGHT: when not scanning select the channel to see it's frequency. Short press steps 1, long 10.

UP / DOWN: change the time spent in a channel. Suggested: 150 us, but you can play around.

## Warning
These apps are for **educational purposes** only. Please use this code responsibly and only use these apps on your own equipment.


## Compared to HackRf

<img src="https://raw.githubusercontent.com/htotoo/NRF24ChannelScanner/main/NRF24ChannelScanner/.flipcorg/banner.png">

Thanks for the image: [Sil333033](https://github.com/Sil333033)


# Pinout from from NoComp/Frog
<img src="https://media.discordapp.net/attachments/937479784726949900/994495234618687509/unknown.png?width=567&height=634">

# Mousejacker / NRF24 pinout by UberGuidoZ
2/A7 on FZ goes to MOSI/6 on nrf24l01<br>
3/A6 on FZ goes to MISO/7 on nrf24l01<br>
4/A4 on FZ goes to CSN/4 on nrf24l01<br>
5/B3 on FZ goes to SCK/5 on nrf24l01<br>
6/B2 on FZ goes to CE/3 on nrf24l01<br>
8/GND on FZ goes to GND/1 on nrf24l01<br>
9/3V3 on FZ goes to VCC/2 on nrf24l01<br>
IRQ/8 is left disconnected on nrf24l01<br>

![NRF_Pins](https://user-images.githubusercontent.com/57457139/178093717-39effd5c-ebe2-4253-b13c-70517d7902f9.png)

If the nRF module is acting a bit flakey, try adding a capacitor to the vcc/gnd lines! 
I've not tried the Plus model so it may have a bigger need for a cap. 
Otherwise, I haven't had any major issues. 
Anything from a 3.3 uF to 10 uF should do. (Watch your positive/negative placement! Negative to ground.) 
I learned if you wanna get fancy, include a 0.1 uF cap in parallel. 
The 3.3 uF to 10 uF will respond to slow freq changes while the 0.1 uF will respond to the high freq switching spikes that the larger one cannot. That said, a single 10 uF will likely suffice for the Mousejack attack. ¯\\\_(ツ)_/¯

![NRF_Capacitor](https://user-images.githubusercontent.com/57457139/178169959-d030f9a6-d2ac-46af-af8b-470ff092c8a7.jpg)

## Disclaimer
Used images and some text from this repo: https://raw.githubusercontent.com/RogueMaster/flipperzero-firmware-wPlugins/420/documentation/NRF24.md
Thanks for them for their work!
