# flipperzero-atomicdiceroller
🎲☢ An atomic dice roller for the Flipper Zero ☢🎲

![banner](https://github.com/nmrr/flipperzero-atomicdiceroller/blob/main/img/banner.jpg)

[**Geiger Counter**](https://github.com/nmrr/flipperzero-geigercounter) application must work on your **Flipper Zero** to be able to use this application. You need the same **geiger board** and this board must be connected in the same way as described in the description of the **Geiger Counter** application.

**Note:** There is no test port compared to the **Geiger Counter** application, your absolutely need a **geiger board** to run this application.

This application generates random numbers by hashing timestamps from a 64 MHz 32-bit time clock (TIM2) from the **MCU**. Two hash methods have been implemented:
- CRC32: 8 ticks are needed to obtain a hash, for low activity sources
- MD5: 32 ticks are needed to obtain a hash, for high activity source

Dice rolls are produced by transforming hashes into 1 to 6 numbers. Output is balanced by ignoring out of scope values. Modulo-based methods are ugly because they are usually unbalanced.

It's possible to roll the dice without using a **radioactive isotope**. Air contains **radon** gas that is **radioactive**. **Geiger board** can detect descendants of radon gas that emit strong **beta** or **gamma** rays.

## Gallery

<img src="https://github.com/nmrr/flipperzero-atomicdiceroller/blob/main/img/flipper2.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-atomicdiceroller/blob/main/img/flipper3.png" width=25% height=25%>

In the left corner, **counts per second** (cps) indicates the activity. In the right corner, **availiable dice rolls** are indicated. 64 rolls can be stored.

## Build the program

Assuming the toolchain is already installed, copy **flipper_atomicdiceroller** directory to **applications_user**

Plug your **Flipper Zero** and build the atomic dice roller:
```
./fbt launch_app APPSRC=applications_user/flipper_atomicdiceroller
```

The program will automatically be launched after compilation

<img src="https://github.com/nmrr/flipperzero-atomicdiceroller/blob/main/img/flipper1.png" width=25% height=25%>

**Button assignments**: 

button  | function
------------- | -------------
**Ok** *[short short]*  | Roll the dice
**Left** *[long press]* | Set CRC32 as hash method
**Right** *[long press]* | Set MD5 as hash method
**Back** *[long press]*  | Exit

If you don't want to build this application, just simply copy **flipper_atomicdiceroller.fap** on your **Flipper Zero** 

Build has been made with official toolchain, **API Mismatch** error may appear if you are using custom firmware. You can bypass this error but the program may crash.
