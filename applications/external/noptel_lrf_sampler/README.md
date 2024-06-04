# Noptel LRF rangefinder sampler app for the Flipper Zero
### Version 1.9

* [Usage](#Usage)
* [Installation](#Installation)
* [Notes](#Notes)
* [License](#License)

Flipper Zero app to get range measurements from a [Noptel](https://noptel.fi/) [LRF rangefinder](https://noptel.fi/rangefinderhome).

#### Functions

- Rangefinding in all modes
- Distance and amplitude averaging
- Return rate display
- Laser pointer control
- LRF information display
- Saving diagnostic data
- Rangefinder laser testing
- IR laser pointer testing
- USB serial passthrough with serial traffic display

https://github.com/Giraut/flipper_zero_noptel_lrf_sampler/assets/37288252/32f0bf66-ff2f-47e1-9552-5453781482de



## Usage

Start the application in **Apps ▶ GPIO**.

The main menu opens:

![Main menu](screenshots/11-main_menu.png)


### Configuration

![Configuration menu](screenshots/9-configuration_menu.png)

In the configuration menu, select the sampling mode:

- **SMM**: single measurement mode (default)
- **Auto SMM**: single measurement mode, auto-repeating
- **1 Hz** ▶ **200 Hz**: continuous measurement mode at the selected sampling rate

Set **Buffering** to buffer samples in automatic SMM or continuous measurement mode for either:

- **None**: no buffering (default)
- **1 s** ▶ **10 s**
- **5 samples**, **10 samples**, **100 samples** or **1000 samples**

Enable **Beep** to hear a short beep when a valid sample is received.

Set **Baudrate** to either:

- **115200** bps (default)
- **57600** bps
- **38400** bps
- **19200** bps
- **9600** bps

For USB serial passthrough COM port management, set **Passthru channel** to either:

- **0**: the single virtual COM port normally used by the Flipper Zero is repurposed to relay data to / from the LRF
- **1**: a second virtual COM port is created specifically to relay data to / from the LRF

When using channel 0, the Flipper Zero always appears as one single COM port. However, qFlipper and the CLI become unavailable in the passthrough view.

When using channel 1, depending on whether the passthrough view is started or not, the Flipper Zero appears as one or two COM ports. qFlipper and the CLI remain available on the lower COM port (channel 0).

*See "Serial protocol debugging" below*


### Sample

Select the **Sample** option to start sampling the LRF.

#### Single measurement mode

In single measurement mode, press the **OK** button to get another measurement.

![Sampling in single measurement mode](screenshots/12-sample_smm.png)

Up to 3 distances can be measured in one sample (several targets in the line of sight).

Each measured distance is displayed on one line.

The associated amplitude - i.e. signal strength - is displayed as a small number next to the distance.

#### Continuous measurement mode

In continuous measurement mode or automatic SMM mode, press the **OK** button to start and stop the measurements.

![Sampling in continuous measurement mode](screenshots/1-sample_cmm.png)

Distances and amplitudes are displayed like in single measurement mode.

The effective sampling rate in Hertz is calculated and displayed at the bottom.

#### Buffering

If buffering is enabled, the following information is calculated:

- Average distances for all valid samples
- Average amplitudes - i.e. average signal strengths - for all valid samples
- Return rate - i.e. number of samples containing any measured distance vs. total number of samples

The average distances and amplitudes are displayed instead of the last sample's.

![Sample buffering](screenshots/0-sample_buffering.png)

The buffering state is displayed at the bottom left: the bar is all the way up when the buffer is full.

The return rate is indicated next to the buffering state: the bar is all the way up when the return rate is 100% - meaning no missed measurements.

![Sample buffering bars](screenshots/17-sample_buffering_bars.png)

The buffering state and the return rate indicators are useful to determine the rangefinder's extinction ratio: when the buffer is full, the return rate bar should be only about ½ to ¾ full at extinction ratio.

https://github.com/Giraut/flipper_zero_noptel_lrf_sampler/assets/37288252/e55122ff-178d-43d3-911b-0656ed161fe8

### Pointer ON/OFF

Select the **Pointer ON/OFF** toggle to turn the pointer on and off if the rangefinder is equipped with a pointer.

![Pointer ON/OFF toggle](screenshots/13-pointer_on_off_toggle.png)

The pointer is useful to train the rangefinder on a short distance target.

### LRF information

Select the **LRF information** option to view the LRF's identification information. Press the **OK** button to re-read the information, or read another LRF's.

![GPIO pin connections](screenshots/2-lrf_information.png)

### Save LRF diagnostic

Select the **Save LRF diagnostic** option to save the LRF's diagnostic data after a failed ot incorrect measurement. Press the **OK** button to save another set of diagnostic data or another LRF's.

![Save LRF diagnostic](screenshots/8-save_lrf_diagnostic.png)

DSP files may be submitted to Noptel for analysis, along with a description of the problem with the rangefinder.

To recover the DSP files, connect the Flipper Zero to the computer with a USB cable and use [qFlipper](https://docs.flipper.net/qflipper) to download the the **noptel_lrf_diag** directory:

![Download DSP directory](screenshots/15-download_dsp_directory.png)

DSP files inside the **noptel_lrf_diag** directory may be downloaded individually also:

![Download DSP file](screenshots/16-download_dsp_file.png)

### Test LRX laser

Select the **Test LRX laser** option to test an LRX's transmitter laser.

*See note below*

![Laser testing](screenshots/3-laser_testing.png)

### Test IR pointer

Select the **Test IR pointer** option to test an LRF's infrared pointer.

*See note below*

![Pointer testing](screenshots/14-pointer_testing.png)

### USB serial passthrough

The **USB serial passthrough** function lets a computer connected to the Flipper Zero's USB port and running a LRF client software communicate with the LRF as if the LRF was connected directly to the computer with a serial cable. The Flipper Zero transparently relays data between the computer and the LRF.

The first screen shows the overall amount of serial traffic going to and from the LRF:

![Serial passthrough - Traffic volume](screenshots/4-usb_serial_passthrough1.png)

The second screen, accessible with the right arrow, shows the actual traffic: bytes sent to the LRF are showed in black while bytes returned by the LRF are showed normally.

![Serial passthrough - Transferred bytes](screenshots/5-usb_serial_passthrough2.png)

*See "Serial protocol debugging" below*

### About

Gives information about the app and the GPIO pin connections needed to connect an LRF rangefinder to the Flipper Zero. Use the **OK** button or the arrows to switch pages.

![App version](screenshots/6-splash_version.png)

![App description](screenshots/10-app_description.png)

![GPIO pin connections](screenshots/7-gpio_pin_connections.png)



## Serial protocol debugging

If you are developing an LRF application or you're troubleshooting the communication between your computer and the rangefinder, you can of course use the second screen of the **USB serial passthrough** function to view the live serial data traffic on the Flipper Zero's screen.

But for more comfortable and more advanced debugging, you can also capture the traffic as a log trace in the Flipper Zero's command line interface (CLI).

To use this functionality:

- Configure **Passthru channel** to **1**, so that the CLI remains available on channel 0
- Enter the **USB serial passthrough** view
- Start a terminal on the serial device corresponding to channel 0 (e.g. `/dev/ttyACM0` in Linux, `COM1` in Windows) preferrably at the highest possible speed
- In the terminal, enable trace logging using the command `log trace`
- Use the serial device corresponding to channel 1 (e.g. `/dev/ttyACM1` in Linux, `COM2` in Windows) to interact with the LRF as you would with a regular serial cable. The terminal should show the serial traffic going to and from the LRF like this:

```
              _.-------.._                    -,
          .-"```"--..,,_/ /`-,               -,  \
       .:"          /:/  /'\  \     ,_...,  `. |  |
      /       ,----/:/  /`\ _\~`_-"`     _;
     '      / /`"""'\ \ \.~`_-'      ,-"'/
    |      | |  0    | | .-'      ,/`  /
   |    ,..\ \     ,.-"`       ,/`    /
  ;    :    `/`""\`           ,/--==,/-----,
  |    `-...|        -.___-Z:_______J...---;
  :         `                           _-'
 _L_  _     ___  ___  ___  ___  ____--"`___  _     ___
| __|| |   |_ _|| _ \| _ \| __|| _ \   / __|| |   |_ _|
| _| | |__  | | |  _/|  _/| _| |   /  | (__ | |__  | |
|_|  |____||___||_|  |_|  |___||_|_\   \___||____||___|

Welcome to Flipper Zero Command Line Interface!
Read the manual: https://docs.flipper.net/development/cli
Run `help` or `?` to list available commands

>: log trace
Current log level: trace
Use <log ?> to list available log levels
Press CTRL+C to stop...
38236253 [T][noptel_lrf_sampler] >LRF: c6 96
38236275 [T][noptel_lrf_sampler] <LRF: 59 c6 3c 0b
38236777 [T][noptel_lrf_sampler] >LRF: cc 00 00 00 9c
38238098 [T][noptel_lrf_sampler] <LRF: 59 cc ca 6f 80 42 2f 01 00 00 01 20 00
38238098 [T][noptel_lrf_sampler] <LRF: 00 00 00 01 20
38238102 [T][noptel_lrf_sampler] <LRF: 00 00 00 c2
```

### lrf_traffic_tracer.py utility

Instead of accessing the Flipper Zero CLI with a terminal and enabling the trace log manually, you can use the **lrf_traffic_tracer.py** utility to transparently connect to the CLI and decode the LRF traffic in real time.

If Noptel's LRF Python class is installed and imported, by default, the utility will fully decode the traffic:

```
$ python lrf_traffic_tracer.py /dev/ttyACM0

LRF traffic tracer
------------------

Using the full LRF frame decoder
Ctrl-C to exit...

/dev/ttyACM0 opened at 921600 bps

Trace log started

26475365: <LRF: cmd=CMM_BREAK
26475389: >LRF: cmd=RESP_CMM_BREAK
26475890: <LRF: cmd=EXEC_RANGE_MEAS, measurementmode=0, extradelaybwpulses=0, burstdivider=0
26477240: >LRF: cmd=RESP_EXEC_RANGE_MEAS, range1=0.000, signallevel1=0, range2=0.000, signallevel2=0, range3=0.000, signallevel3=0, statusbyte3=32
```

If Noptel's LRF Python class is not available, the utility will perform basic recognition of LRF commands and responses using a simple standalone decoder:

```
$ python lrf_traffic_tracer.py /dev/ttyACM0

LRF traffic tracer
------------------

Using the standalone LRF frame decoder
Ctrl-C to exit...

/dev/ttyACM0 opened at 921600 bps

Trace log started

26620715: <LRF: CMM_BREAK
26620739: >LRF: RESP_CMM_BREAK
26621240: <LRF: EXEC_RANGE_MEAS
26622586: >LRF: RESP_EXEC_RANGE_MEAS
```

If you want to see the raw LRF serial traffic without decoding it, use the `-r` command line switch:

```
$ python lrf_traffic_tracer.py /dev/ttyACM0 -r

LRF traffic tracer
------------------

Ctrl-C to exit...

/dev/ttyACM0 opened at 921600 bps

Trace log started

26769739: <LRF: c6 96 59 c6 3c 0b
26770264: <LRF: cc 00 00 00 9c
26771604: >LRF: 59 cc 00 00 01 20 00 00 00 00 01 20 00 00 00 00 01 20 00 00 20
26771610: >LRF: f8
```

*Note: the **lrf_traffic_tracer.py** utility requires at least Python 3 and the pySerial module*



## Installation

### Pre-built app

Install this app from https://lab.flipper.net/apps/noptel_lrf_sampler

### Manually

- Clone this repository
- Install [uFBT](https://github.com/flipperdevices/flipperzero-ufbt)
- Connect the Flipper Zero to the computer with a USB cable
- cd into the repository
- Invoke `ufbt launch` to install the app in the **Apps ▶ GPIO** menu and run it for the first time



## Notes

- Pin #15 (C1) may be used to control the LRF's power supply through a power MOSFET or a relay for example. If used, the app will turn on the rangefinder upon starting and turn it back off when exiting. This is useful if the LRF is battery-powered to keep the rangefinder connected to the Flipper Zero and conserve the battery when not in use. The use of this pin is entirely optional.

- LRX-series rangefinders may also be used with the app. However, they are usually configured as RS232 or RS422 serial devices, not as UART serial devices. Therefore, a UART-to-RS232 or UART-to-RS422 converter is usually required to connect the Flipper Zero's UART lines to an LRX rangefinder.

- Only certain rangefinger models' lasers can be tested with the **Test LRX laser** function, most notably LRX-class rangefinders - hence the name of the function - but also certain LRF rangefinders. Depending on the particular model, despite nominally working in the far-infrared, 1.5 µm lasers can leak near-infrared light around 950 nm that can be picked up by the Flipper Zero's [infrared receiver](https://www.vishay.com/docs/82495/tsop753.pdf) at close range.

    Your particular rangefinder may be detected by the Flipper Zero, but there is no guarantee that it will be, as this function exploits a side effect of the laser diode that the rangefinders' design doesn't account for. If your rangefinder's laser isn't detected, do not assume it is malfunctioning! If it is detected however, you can be quite sure it is functioning well.

    LRF rangefinders such as the LRF125 that work in the near-infrared are easily detected by the Flipper Zero despite using a lower power laser diode, and can be reliably diagnosed using the Flipper Zero.

- Some rangefinders are equipped with a low power, near-infrared pointer instead of a visible light pointer. Near-infrared pointers can be easily detected and reliably diagnosed by the Flipper Zero also, using the **Test IR pointer** function.

- Each sample requires 220 bits to transfer over serial. If the baudrate is too low, the serial link may not provide enough bandwidth to sustain the desired rate of sampling:

  - At 38400 bps, the sampling rate is capped at 174 Hz
  - At 19200 bps, the sampling rate is capped at 87 Hz
  - At 9600 bps, the sampling rate is capped at 43.5 Hz

- The lower the baudrate, the longer saving diagnostic data takes. At 9600 bps, it takes upward of 50 seconds, while it only takes 5 seconds at 115200 bps



## License

MIT
