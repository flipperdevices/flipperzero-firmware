## flipperscope

Oscilloscope application - apply signal to pin 16/PC0, with a voltage ranging from 0V to 2.5V and ground to pin 18/GND.

## Guide

**Center** = Pause/Un-pause display

* In the setup screen you specify a time period of the analogue to digital converter, so 1ms, means sampling at 1000Hz.

* Setup screen enables you to choose to measure the frequency of a signal with the Time option, in Hertz.

* Setup screen enables you to choose to measure the maximum, minimum and peak-to-peak voltage, with the Voltage option.

* Setup screen also enables you to choose the capture mode, to save samples to the SD card (currently 128 samples).  You can
parse this data using the Python script in the flipperscope repo.
