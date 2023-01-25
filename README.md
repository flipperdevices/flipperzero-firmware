# flipperscope

To compile and install

```
cd flipperzero-firmware/applications_user
git clone https://github.com/anfractuosity/flipperscope.git
cd ..
./fbt DEBUG=1 VERBOSE=1 fap_scope
./fbt launch_app APPSRC=applications_user/flipperscope
```

Provide signal to pin 16/C0, with a voltage ranging from 0V to 3.3V.

## To Do

* Customisable input pin
* Check time period is set correctly
* Test with signal generator
* Trigger type mode
* Way to select and measure voltage from waveform
* FFT
* ...

## Inspiration

* For GUI menu system, used code principles from weather station and signal generator apps
* STM32 DMA example
* VREFBUF information - https://community.st.com/s/question/0D53W00001awIlMSAU/enable-and-use-vrefbuf-for-adc-measures
* Relocating vector table - https://community.nxp.com/t5/i-MX-Processors/Relocate-vector-table-to-ITCM/m-p/1302304
