# flipperzero-chronometer
⏱️⏱️ A chronometer application for the Flipper Zero ⏱️⏱️

![banner](https://github.com/nmrr/flipperzero-chronometer/blob/main/img/bannerwatch.jpg)

This chronometer is accurate to the millisecond. **TIM2** internal timer of the **STM32** MCU is used to generate a 64 MHz clock signal. This signal is used to count elapsed time.

## Gallery

<img src="https://github.com/nmrr/flipperzero-chronometer/blob/main/img/chrono2.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-chronometer/blob/main/img/chrono3.png" width=25% height=25%>

**Note:** Chronometer stops after 99 hours, 59 minutes and 59 seconds

## Build the program

Assuming the toolchain is already installed, copy **flipper_chronometer** directory to **applications_user**

Plug your **Flipper Zero** and build the chronometer:
```
./fbt launch_app APPSRC=applications_user/flipper_chronometer
```

The program will automatically be launched after compilation

<img src="https://github.com/nmrr/flipperzero-chronometer/blob/main/img/chrono1.png" width=25% height=25%>

**Button assignments**: 

button  | function
------------- | -------------
**Ok** *[short press]*  | Start/stop the chronometer
**Ok** *[long press]*  | Reset the chronometer
**Back** *[long press]*  | Exit

If you don't want to build this application, just simply copy **flipper_chronometer.fap** on your **Flipper Zero** 

Build has been made with official toolchain, **API Mismatch** error may appear if you are using custom firmware. You can bypass this error but the program may crash.

## Changelog

* 2023-08-08
  * Hour is now displayed
  * Limits have been increased to 99:99:99 instead of 1 hour

* 2023-08-02
  * Initial release
