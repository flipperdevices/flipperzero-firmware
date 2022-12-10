# Flipper POCSAG Receiver plugin

### Protocol implementation made by @Shmuma
Plugin based on [Weather Station](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/plugins/weather_station) from OFW


Includes two FM presets built into code - with 9.5khz and 15khz deviation

To add new presets and frequencies create file `yourMicroSD/pocsag/settings.txt`
And put [THIS](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/assets/resources/subghz/assets/setting_user.example) file contents into it, and edit this example for yourself, add needed frequencies


If you want to build Plugin straight from the repo - its currently compatible only with Unleashed firmware
To build for OFW please comment line 9 in `pocsag_pager/pocsag_pager_app.c`


All contributions are welcome!
