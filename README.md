# Flipper POCSAG Receiver plugin

### Protocol implementation made by @Shmuma + 512, 2400 speeds support by @htotoo
Plugin based on [Weather Station](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/plugins/weather_station) from OFW

Icons by @Svaarich

Includes new FM preset built into code - 2FSK with 9.5KHz freq deviation.

App supports POCSAG 512, POCSAG 1200, POCSAG 2400 decoding on CC1101 supported frequencies! 
Check datasheet and add required frequency in config file (see details below)

Default frequency is set to DAPNET - `439987500` 

To add new presets and frequencies create file `yourMicroSD/pocsag/settings.txt`
And put [THIS](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/applications/main/subghz/resources/subghz/assets/setting_user.example) file contents into it, and edit this example for yourself, add needed frequencies


All contributions are welcome!
