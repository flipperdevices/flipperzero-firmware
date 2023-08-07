# flipperscope

To compile and install

```
cd flipperzero-firmware/applications_user
git clone https://github.com/anfractuosity/flipperscope.git
cd ..
./fbt DEBUG=1 VERBOSE=1 fap_scope
./fbt launch_app APPSRC=applications_user/flipperscope
```

Provide signal to **pin 16/PC0**, with a voltage ranging from 0V to 2.5V and ground to **pin 18/GND**.

Press the 'ok' button (button in the centre of joypad) to pause/unpause the waveform display.

[Demo](https://www.youtube.com/watch?v=tu2X1WwADF4) showing three different waveform types from a signal generator.

Also see [Derek Jamison's demonstration](https://www.youtube.com/watch?v=iC5fBGwCPHw&t=374s) of this app as well as other interesting projects.

## Measurements

* Measures frequency of waveform in hertz
* Measures voltage: min, max, Vpp

![Signal Generator](photos/sig.jpg)

![Flipper Zero running flipperscope](photos/freq.jpg)

![Rigol](photos/rigol.jpg)

![Flipper Zero running flipperscope](photos/volt.jpg)

## To Do

* Customisable input pin
* Trigger type mode
* FFT
* ...

## Inspiration

* For GUI menu system, used code principles from weather station and signal generator apps
* STM32 DMA example
* VREFBUF information - https://community.st.com/s/question/0D53W00001awIlMSAU/enable-and-use-vrefbuf-for-adc-measures
* Relocating vector table - https://community.nxp.com/t5/i-MX-Processors/Relocate-vector-table-to-ITCM/m-p/1302304
