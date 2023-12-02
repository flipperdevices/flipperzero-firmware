# Flipper Zero calendar application

- [Flipper Zero Official Website](https://flipperzero.one). A simple way to explain to your friends what Flipper Zero can do.
- [Flipper Zero Firmware Update](https://update.flipperzero.one). Improvements for your dolphin: latest firmware releases, upgrade tools for PC and mobile devices.
- [User Documentation](https://docs.flipperzero.one). Learn more about your dolphin: specs, usage guides, and anything you want to ask.

## How to set up and build the application

Make sure you have enough space on SD card and clone the source code inside `applications_user` folder:

```shell
git clone https://github.com/Adiras/flipperzero-calendar.git
```

- To launch app on Flipper, run `./fbt launch APPSRC=applications_user\flipperzero-calendar`
- To build app without uploading it to Flipper, use `./fbt build APPSRC=applications_user\flipperzero-calendar`
