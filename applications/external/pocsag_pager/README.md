# Flipper POCSAG Receiver plugin

### Protocol implementation made by @Shmuma + 512, 2400 speeds support by @htotoo
Plugin based on [Weather Station](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/plugins/weather_station) from OFW

Icons by @Svaarich

Includes new FM preset built into code - 2FSK with 9.5KHz freq deviation.

App supports POCSAG 512, POCSAG 1200, POCSAG 2400 decoding on CC1101 supported frequencies! 
Check datasheet and add required frequency in config file (see details below)

Default frequency is set to DAPNET - `439987500` 

To add new presets and frequencies create file `yourMicroSD/pocsag/settings.txt` <br>
And put [THIS](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/applications/main/subghz/resources/subghz/assets/setting_user.example) file contents into it, and edit this example for yourself, add needed frequencies <br>
Warning!!! This file is EXAMPLE! It contains frequencies that are commented with `#` <br>
`#` Is a comment symbol, if that symbol is present at the beggining of the line, that means this line will be ignored! <br>
To add custom frequency you need to uncomment the line and edit numbers keeping its size (zeros at the end) `Frequency: 433000000` <br>
To edit default freqency follow same procedure <br>
All custom frequencies added from this file will be at the end of the list in the frequency selector in the app! Scroll frequencies to the end to find your custom freq. <br>
Default list > Custom list



All contributions are welcome!
