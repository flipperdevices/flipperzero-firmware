# SubGHz Bruteforcer Application for Flipper Zero

![image](https://github.com/DarkFlippers/flipperzero-subbrute/assets/31771569/0a952559-94c0-40b7-8b9c-9ee4e37c6cd6)


SubGhz Bruteforcer from [Unleashed Firmware](https://github.com/DarkFlippers/unleashed-firmware)

## Disclaimer

This software is for experimental purposes only and is not meant for any illegal activity/purposes.
We do not condone illegal activity and strongly encourage keeping transmissions to legal/valid uses allowed by law.

---

## Installation

The application is included in the standard firmware package of [Unleashed Firmware](https://github.com/DarkFlippers/unleashed-firmware). 
You just need to install the [latest firmware](https://github.com/DarkFlippers/unleashed-firmware/releases/latest).
<br />
You can also download the [release](https://github.com/derskythe/flipperzero-subbrute/releases/latest) and unzip/untar it to the `SD Card/apps/Sub-GHz` directory.
<br />
> [!WARNING]
> 
> The application is not compatible with the official firmware version.
> Also, it has not been tested on other firmware versions other than [Unleashed Firmware](https://github.com/DarkFlippers/unleashed-firmware) and [OFW](https://github.com/flipperdevices/flipperzero-firmware).

## User Guide

### Main Menu

![image](https://github.com/DarkFlippers/flipperzero-subbrute/assets/31771569/9f428d6e-59fd-4517-895d-fb185f8d884f)

Here you can select the protocol and frequency that will be used for bruteforce. 
According to our observations, `CAME 12bit 433MHz` is the most protocol, so it is selected by default.
To identify other devices and protocols, you should inspect the device.
According to the protocol, when probe a key, each value is sent 3 times. 
Most of the devices this works but there are devices that don't work and more repetitions are needed. 
The number of repetitions can be increased with the right button, the left button decreases the value.
The negative side of increasing the number of repetitions will be a longer key find time.

![image](https://github.com/DarkFlippers/flipperzero-subbrute/assets/31771569/a17695d0-833d-44ce-a0d0-ead8366cb4fe)

#### Buttons

|           Button            |        Action         |
|:---------------------------:|:---------------------:|
|      :arrow_backward:       | Decrease repeat value |
|      :arrow_up_small:       |        Move up        |
|       :arrow_forward:       | Increase repeat value |
|     :arrow_down_small:      |       Move down       |
|       :record_button:       |    Select protocol    |
| :leftwards_arrow_with_hook: |   Close application   |

## Supported Protocols

![image](https://github.com/DarkFlippers/flipperzero-subbrute/assets/31771569/1f14b5eb-7e66-4b37-b816-34fab63db70c)


### CAME

- CAME 12bit 303MHz
- CAME 12bit 307MHz
- CAME 12bit 315MHz
- CAME 12bit 433MHz
- CAME 12bit 868MHz

### NICE

- NICE 12bit 433MHz
- NICE 12bit 868MHz

### Ansonic

- Ansonic 12bit 433.075MHz
- Ansonic 12bit 433.920MHz
- Ansonic 12bit 434.075MHz

### Holtek

- Holtek HT12X 12bit FM 433.920MHz (TE: 204μs)
- Holtek HT12X 12bit AM 433.920MHz (TE: 433μs)
- Holtek HT12X 12bit AM 315MHz (TE: 433μs)
- Holtek HT12X 12bit AM 868MHz (TE: 433μs)
- Holtek HT12X 12bit AM 915MHz (TE: 433μs)

### Chamberlain

- Chamberlain 9bit 300MHz
- Chamberlain 9bit 315MHz
- Chamberlain 9bit 390MHz
- Chamberlain 9bit 433MHz
- Chamberlain 8bit 300MHz
- Chamberlain 8bit 315MHz
- Chamberlain 8bit 390MHz
- Chamberlain 7bit 300MHz
- Chamberlain 7bit 315MHz
- Chamberlain 7bit 390MHz

### Linear

- Linear 10bit 300MHz
- Linear 10bit 310MHz
- Linear Delta 3 8bit 310MHz

### UNILARM

> [!NOTE]
> 
> Only dip switch combinations, not full 25bit bruteforce

- UNILARM 25bit 330MHz (TE: 209μs)
- UNILARM 25bit 433MHz (TE: 209μs)

### SMC5326

> [!NOTE]
> 
> Only dip switch combinations, not full 25bit bruteforce

- SMC5326 25bit 330MHz (TE: 320μs)
- SMC5326 25bit 433MHz (TE: 320μs)

### PT2260

> [!NOTE]
> 
> Only for 8 dip switch remote, not full 24bit bruteforce

- PT2260 24bit 315MHz (TE: 286μs)
- PT2260 24bit 330MHz (TE: 286μs)
- PT2260 24bit 390MHz (TE: 286μs)
- PT2260 24bit 433MHz (TE: 286μs)

### Additional

- BF Existing dump works for most other static protocols supported by Flipper Zero
