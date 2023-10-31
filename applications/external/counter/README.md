# Dolphin Better Counter
This is a simple external application for the [Flipper Zero](https://www.flipperzero.one).
It gives you access to a counter which you can increment using the center, up or right buttons. You can decrement the counter using the down or left buttons.

![preview1](media/1.png)
![preview2](media/2.png)

# How to install this?
- Clone this repository and copy the directory and it's contents to `<firmware directory>/applications_user/`
- Run `cd <firmware directory> && ./fbt fap_better_counter`
- Upload application binary `better_counter.fap` (e.g. at `<firmware directory>/build/f7-firmware-D/.extapps/better_counter.fap`) to your Flipper Zero, or you can upload the precompiled binary found at `/build/<API_VERSION>/better_counter.fap`
